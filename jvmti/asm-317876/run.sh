#!/bin/bash -exu

if [ -z $JAVA_HOME ]; then
	echo "Please set JAVA_HOME."
	exit 0
fi

# Run the sample agent and jar
#
mkdir -p out
rm -f out/*.class
$JAVA_HOME/bin/java -agentpath:$PWD/agent/build/libagent.so -jar $PWD/java_hello/build/java_hello.jar

# Decompile the test classes.
# Notice that in the first command output, StackMapTable is present, and in the second one, it isn't.
#
$JAVA_HOME/bin/javap -p -l -v -s -c out/java_util_Date_loaded.class | grep StackMapTable
$JAVA_HOME/bin/javap -p -l -v -s -c out/java_util_Date_redefined.class | grep StackMapTable
