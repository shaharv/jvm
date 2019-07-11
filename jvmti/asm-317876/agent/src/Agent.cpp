#include "Agent.h"

#include <iostream>

using std::cerr;
using std::cout;
using std::endl;

Agent* Agent::agent = NULL;

Agent::Agent(jvmtiEnv* jvmti) :
	_jvmtiEnv(jvmti),
	_jvmtiCapabilities(),
	_jvmtiCallbacks(),
	_valid(false)
{
	cout << "Hello from JVMTI agent!" << endl;

	_valid = initialize();

	if (!_valid)
	{
		cerr << "Agent initialization failed!" << endl;
	}
}

Agent::~Agent()
{

}

bool Agent::initialize()
{
	return ((initializeCapabilities()) &&
		(initializeCallbacks()));
}

static void JNICALL classBytesLoaded(jvmtiEnv*,
	JNIEnv*,
	jclass,
	jobject,
	const char* name,
	jobject,
	jint classBytesLength,
	const unsigned char* classBytes,
	jint* newClassBytesLength,
	unsigned char** newClassBytes)
{
	cout << "Loading bytes for class: " << name << endl;
}

bool Agent::initializeCapabilities()
{
	_jvmtiCapabilities.can_generate_all_class_hook_events = JVMTI_ENABLE;

	jvmtiError error = _jvmtiEnv->AddCapabilities(&_jvmtiCapabilities);

	return (error == JVMTI_ERROR_NONE);
}

bool Agent::initializeCallbacks()
{
	jvmtiError error = _jvmtiEnv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_CLASS_FILE_LOAD_HOOK, NULL);

	if (error != JVMTI_ERROR_NONE)
	{
		return false;
	}

	_jvmtiCallbacks.ClassFileLoadHook = &classBytesLoaded;

	error = _jvmtiEnv->SetEventCallbacks(&_jvmtiCallbacks, (jint)sizeof(_jvmtiCallbacks));

	return (error == JVMTI_ERROR_NONE);
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

	return 0;
}

JNIEXPORT void JNICALL Agent_OnUnload(JavaVM*)
{
	cout << "VM death started. Goodbye." << endl;

	delete Agent::agent;
}
