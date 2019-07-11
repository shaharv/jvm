#include "Agent.h"

#include <iostream>

Agent* Agent::agent = NULL;

Agent::Agent(jvmtiEnv* jvmti) :
	_jvmtiEnv(jvmti)
{
	std::cout << "Hello from JVMTI agent!" << std::endl;
}

Agent::~Agent()
{

}

JNIEXPORT jint JNICALL Agent_OnLoad(JavaVM* jvm, char *options, void*)
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

JNIEXPORT void JNICALL Agent_OnUnload(JavaVM *vm)
{
	std::cout << "VM death started. Goodbye." << std::endl;

	delete Agent::agent;
}
