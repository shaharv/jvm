#pragma once

#include <jvmti.h>

#include <set>
#include <string>
#include <vector>

class Agent
{
public:
	static Agent* agent;

	Agent(jvmtiEnv* jvmti);
	~Agent();

	static void JNICALL vmInit(jvmtiEnv*, JNIEnv*, jthread);
	static void JNICALL classLoaded(jvmtiEnv*, JNIEnv*, jthread, jclass klass);
	static void JNICALL classBytesLoaded(jvmtiEnv*, JNIEnv*, jclass classBeingRedefined, jobject, const char* name,
		jobject, jint classBytesLen, const unsigned char* classBytes, jint* newClassBytesLen,
		unsigned char** newClassBytes);

	bool initialize();

private:
	bool initializeCapabilities();
	bool initializeCallbacks();
	bool retransformClass(jclass klass);
	std::string getClassName(jclass klass);
	void dumpClass(const std::string& name, const unsigned char* classBytes, jint classBytesLen,
		const std::string& classState);

	jvmtiEnv* _jvmtiEnv;
	jvmtiCapabilities _jvmtiCapabilities;
	jvmtiEventCallbacks _jvmtiCallbacks;

	bool _valid;

	std::set<std::string> _classesToTest;
};
