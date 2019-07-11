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
	bool initializeCapabilities();
	bool initializeCallbacks();

	jvmtiEnv* _jvmtiEnv;
	jvmtiCapabilities _jvmtiCapabilities;
	jvmtiEventCallbacks _jvmtiCallbacks;

	bool _valid;
};
