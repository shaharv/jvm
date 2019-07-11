#pragma once

#include <jvmti.h>

class Agent
{
public:
	static Agent* agent;

	Agent(jvmtiEnv* jvmti);
	~Agent();

	bool initialize();

private:
	jvmtiEnv* _jvmtiEnv;
	jvmtiCapabilities _jvmtiCapabilities;
};
