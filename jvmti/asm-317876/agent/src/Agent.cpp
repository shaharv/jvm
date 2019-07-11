#include "Agent.h"

#include <algorithm>
#include <iostream>

using std::cerr;
using std::cout;
using std::endl;
using std::set;
using std::string;

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

	_classesToTest.insert("java/util/Locale");
}

Agent::~Agent()
{

}

void JNICALL Agent::vmInit(jvmtiEnv*, JNIEnv*, jthread)
{
	cout << "VM initialized." << endl;
}

void JNICALL Agent::classBytesLoaded(jvmtiEnv*,
	JNIEnv*,
	jclass classBeingRedefined,
	jobject,
	const char* name,
	jobject,
	jint classBytesLength,
	const unsigned char* classBytes,
	jint* newClassBytesLength,
	unsigned char** newClassBytes)
{
	if (agent->_classesToTest.count(name) > 0)
	{
		agent->dumpClass(name, classBytes, classBytesLength);

		agent->_classesToRetransform.insert(classBeingRedefined);

		return;
	}
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
	_jvmtiCapabilities.can_retransform_any_class = JVMTI_ENABLE;

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

	_jvmtiCallbacks.VMInit = &vmInit;
	_jvmtiCallbacks.ClassFileLoadHook = &classBytesLoaded;

	error = _jvmtiEnv->SetEventCallbacks(&_jvmtiCallbacks, (jint)sizeof(_jvmtiCallbacks));

	return (error == JVMTI_ERROR_NONE);
}

void Agent::dumpClass(const string& name, const unsigned char* classBytes, jint classBytesLen)
{
	cout << "Dumping bytes for class: " << name << endl;

	string classFilePath = name;
	std::replace(classFilePath.begin(), classFilePath.end(), '/', '_');
	classFilePath.append(".class");

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
