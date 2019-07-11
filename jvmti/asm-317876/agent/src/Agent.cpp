#include "Agent.h"

#include <iostream>

Agent* Agent::agent = NULL;

Agent::Agent(jvmtiEnv* jvmti) :
	_jvmtiEnv(jvmti),
	_jvmtiCapabilities()
{
	std::cout << "Hello from JVMTI agent!" << std::endl;

	initialize();
}

Agent::~Agent()
{

}

bool Agent::initialize()
{
	_jvmtiCapabilities.can_generate_all_class_hook_events = JVMTI_ENABLE;

	jvmtiError error = _jvmtiEnv->AddCapabilities(&_jvmtiCapabilities);

	if (error != JVMTI_ERROR_NONE)
	{
		return false;
	}

	error = _jvmtiEnv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_CLASS_FILE_LOAD_HOOK, NULL);

	if (error != JVMTI_ERROR_NONE)
	{
		return false;
	}

	std::cout << "OK!" << std::endl;

	return true;
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
