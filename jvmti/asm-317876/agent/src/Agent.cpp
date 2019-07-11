#include "Agent.h"

#include <iostream>

Agent* Agent::agent = NULL;

Agent::Agent(jvmtiEnv* jvmti) :
	_jvmtiEnv(jvmti),
	_jvmtiCapabilities(),
	_jvmtiCallbacks(),
	_valid(false)
{
	std::cout << "Hello from JVMTI agent!" << std::endl;

	_valid = initialize();

	if (!_valid)
	{
		std::cout << "Agent initialization failed!" << std::endl;
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

static void JNICALL classBytesLoaded(jvmtiEnv *jvmti_env,
	JNIEnv* jni_env,
	jclass class_being_redefined,
	jobject loader,
	const char* name,
	jobject protection_domain,
	jint class_data_len,
	const unsigned char* class_data,
	jint* new_class_data_len,
	unsigned char** new_class_data)
{
	std::cout << "Loading bytes for class: " << name << std::endl;
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
		std::cerr << "ERROR: Unable to get JVMTI environment." << std::endl;
		return JNI_ERR;
	}

	Agent::agent = new Agent(jvmti);

	return 0;
}

JNIEXPORT void JNICALL Agent_OnUnload(JavaVM*)
{
	std::cout << "VM death started. Goodbye." << std::endl;

	delete Agent::agent;
}
