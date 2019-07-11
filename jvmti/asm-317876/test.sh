#!/bin/bash -exu

if [ -z $JAVA_HOME ]; then
	echo "Please set JAVA_HOME."
	exit 0
fi

# Build the sample JVMTI agent
#
cd agent
rm -rf build
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

# Decompile the test classes.
# Notice that in the first command output, StackMapTable is present, and in the second one, it isn't.
#
$JAVA_HOME/bin/javap -p -l -v -s -c out/java_util_Date_loaded.class | grep StackMapTable
$JAVA_HOME/bin/javap -p -l -v -s -c out/java_util_Date_redefined.class | grep StackMapTable

