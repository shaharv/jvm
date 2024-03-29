#include "Agent.h"

#include <algorithm>
#include <iostream>

using std::cerr;
using std::cout;
using std::endl;
using std::set;
using std::string;
using std::vector;

Agent* Agent::agent = NULL;

Agent::Agent(jvmtiEnv* jvmti) :
	_jvmtiEnv(jvmti),
	_jvmtiCapabilities(),
	_jvmtiCallbacks(),
	_valid(false),
	_classesToTest()
{
	cout << "Hello from JVMTI agent!" << endl;

	_valid = initialize();

	if (!_valid)
	{
		cerr << "Agent initialization failed!" << endl;
	}

	// List of classes to dump bytes for, when first loaded and when redefined
	//
	_classesToTest.insert("java/util/Date");
	_classesToTest.insert("java/text/SimpleDateFormat");
}

Agent::~Agent()
{

}

bool Agent::initialize()
{
	return ((initializeCapabilities()) &&
		(initializeCallbacks()));
}

bool Agent::initializeCapabilities()
{
	_jvmtiCapabilities.can_generate_all_class_hook_events = JVMTI_ENABLE;
	_jvmtiCapabilities.can_retransform_classes = JVMTI_ENABLE;

	jvmtiError error = _jvmtiEnv->AddCapabilities(&_jvmtiCapabilities);

	return (error == JVMTI_ERROR_NONE);
}

bool Agent::initializeCallbacks()
{
	jvmtiError error = _jvmtiEnv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_VM_INIT, NULL);

	if (error != JVMTI_ERROR_NONE)
	{
		return false;
	}

	error = _jvmtiEnv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_CLASS_FILE_LOAD_HOOK, NULL);

	if (error != JVMTI_ERROR_NONE)
	{
		return false;
	}

	error = _jvmtiEnv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_CLASS_PREPARE, NULL);

	if (error != JVMTI_ERROR_NONE)
	{
		return false;
	}

	_jvmtiCallbacks.VMInit = &vmInit;
	_jvmtiCallbacks.ClassFileLoadHook = &classBytesLoaded;
	_jvmtiCallbacks.ClassPrepare = &classLoaded;

	error = _jvmtiEnv->SetEventCallbacks(&_jvmtiCallbacks, (jint)sizeof(_jvmtiCallbacks));

	return (error == JVMTI_ERROR_NONE);
}

bool Agent::retransformClass(jclass klass)
{
	jvmtiError error = agent->_jvmtiEnv->RetransformClasses(1, &klass);

	if (error == JVMTI_ERROR_NONE)
	{
		cout << "Class successfully retransformed: " << klass << endl;
	}
	else
	{
		cerr << "Failed retransforming jclass " << klass << " (error = " << error << ")" << endl;
	}

	return (error == JVMTI_ERROR_NONE);
}

string Agent::getClassName(jclass klass)
{
	char* rawClassName;

	jvmtiError error = agent->_jvmtiEnv->GetClassSignature(klass, &rawClassName, NULL);

	if (error != JVMTI_ERROR_NONE)
	{
		cerr << "Failed getting class name for " << klass << endl;
		return "";
	}

	string className = rawClassName;
	className = className.substr(1, className.length() - 2);

	return className;
}

void Agent::dumpClass(const string& name, const unsigned char* classBytes, jint classBytesLen, const string& classState)
{
	cout << "Dumping bytes for class: " << name << endl;

	string classFilePath = name;
	std::replace(classFilePath.begin(), classFilePath.end(), '/', '_');
	classFilePath.append(classState);
	classFilePath.append(".class");
	classFilePath = "out/" + classFilePath;

	FILE* classFile = fopen(classFilePath.c_str(), "wb");

	if (classFile == NULL)
	{
		cerr << "Failed creating class file in " << classFilePath << endl;
		return;
	}

	size_t writtenBytes = fwrite(classBytes, sizeof(char), classBytesLen, classFile);

	if (writtenBytes != static_cast<size_t>(classBytesLen))
	{
		cerr << "Only " << writtenBytes << " bytes written for " << name << " (" << classBytesLen << " expected)." << endl;
	}
	else
	{
		cout << "Class bytes written to " << classFilePath << endl;
	}

	fclose(classFile);
}

void JNICALL Agent::vmInit(jvmtiEnv*, JNIEnv*, jthread)
{
	cout << "VM initialized." << endl;
}

void JNICALL Agent::classLoaded(jvmtiEnv*, JNIEnv*, jthread, jclass klass)
{
	const string className = agent->getClassName(klass);

	// We retransform the class after it has been loaded, so we could test the new class bytes
	//
	if (agent->_classesToTest.count(className) > 0)
	{
		agent->retransformClass(klass);
	}
}

void JNICALL Agent::classBytesLoaded(jvmtiEnv*, JNIEnv*, jclass classBeingRedefined, jobject, const char* name, jobject,
	jint classBytesLen, const unsigned char* classBytes, jint* newClassBytesLen, unsigned char** newClassBytes)
{
	if (agent->_classesToTest.count(name) == 0)
	{
		return;
	}

	// We'll get here twice: once with the original bytes, and once with the redefined bytes
	//
	if (classBeingRedefined != NULL)
	{
		agent->dumpClass(name, classBytes, classBytesLen, "_redefined");

		// Use the original bytes for the redefinition
		//
		*newClassBytesLen = classBytesLen;
		*newClassBytes = const_cast<unsigned char*>(classBytes);
	}
	else
	{
		agent->dumpClass(name, classBytes, classBytesLen, "_loaded");
	}
}

JNIEXPORT jint JNICALL Agent_OnLoad(JavaVM* jvm, char*, void*)
{
	jvmtiEnv* jvmti;
	jint res = jvm->GetEnv((void**)&jvmti, JVMTI_VERSION_1_2);

	if ((res != JNI_OK) || (jvmti == NULL))
	{
		cerr << "ERROR: Unable to get JVMTI environment." << endl;
		return JNI_ERR;
	}

	Agent::agent = new Agent(jvmti);

	return JNI_OK;
}

JNIEXPORT void JNICALL Agent_OnUnload(JavaVM*)
{
	cout << "VM death started. Goodbye." << endl;

	delete Agent::agent;
}
