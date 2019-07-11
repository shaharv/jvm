#pragma once

#include <jvmti.h>

class Agent
{
public:
	static Agent* agent;

	Agent(jvmtiEnv* jvmti);
	~Agent();

	bool initialize();

	static void JNICALL classBytesLoaded(jvmtiEnv*,
	        JNIEnv*,
	        jclass,
	        jobject,
	        const char* name,
	        jobject,
	        jint classBytesLength,
	        const unsigned char* classBytes,
	        jint* newClassBytesLength,
	        unsigned char** newClassBytes);

private:
	bool initializeCapabilities();
	bool initializeCallbacks();

	jvmtiEnv* _jvmtiEnv;
	jvmtiCapabilities _jvmtiCapabilities;
	jvmtiEventCallbacks _jvmtiCallbacks;

	bool _valid;
};
