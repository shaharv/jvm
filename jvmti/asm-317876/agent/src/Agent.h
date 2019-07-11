#pragma once

#include <jvmti.h>

class Agent
{
public:
	static Agent* agent;

	Agent(jvmtiEnv* jvmti);
	~Agent();

private:
	jvmtiEnv* _jvmtiEnv;
	JNIEnv* _jniEnv;
};
