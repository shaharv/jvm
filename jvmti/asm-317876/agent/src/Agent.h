#pragma once

#include <jvmti.h>

#include <set>
#include <string>

class Agent
{
public:
	static Agent* agent;

	Agent(jvmtiEnv* jvmti);
	~Agent();

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

	bool initialize();

private:
	bool initializeCapabilities();
	bool initializeCallbacks();
	void dumpClass(const std::string& name, const unsigned char* classBytes, jint classBytesLen);

	jvmtiEnv* _jvmtiEnv;
	jvmtiCapabilities _jvmtiCapabilities;
	jvmtiEventCallbacks _jvmtiCallbacks;

	bool _valid;

	std::set<std::string> _classesToDump;
};
