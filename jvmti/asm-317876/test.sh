#!/bin/bash -exu

if [ -z $JAVA_HOME ]; then
	echo "Please set JAVA_HOME."
	exit 0
fi

# Build the sample JVMTI agent
#
cd agent
rm -r build
./gen-build.sh
cd build
make
cd ../..

# Build the test jar
#
cd java_hello
rm -rf build
mkdir -p build && cd build
$JAVA_HOME/bin/javac -g ../test/Main.java -d .
$JAVA_HOME/bin/jar -cvfe java_hello.jar test.Main test/Main.class

# Run the sample agent and jar
#
cd ../..
rm -rf out
mkdir -p out
$JAVA_HOME/bin/java -agentpath:$PWD/agent/build/libagent.so -jar $PWD/java_hello/build/java_hello.jar
