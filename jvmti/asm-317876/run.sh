#!/bin/bash -exu

if [ -z $JAVA_HOME ]; then
	echo "Please set JAVA_HOME."
	exit 0
fi

# Run the sample agent and jar
#
mkdir -p out
rm -f out/*.class
rm -f out/*.txt
$JAVA_HOME/bin/java -agentpath:$PWD/agent/build/libagent.so -jar $PWD/java_hello/build/java_hello.jar

# Decompile the test classes.
# Notice that in the first command output, StackMapTable is present, and in the second one, it isn't.
#
$JAVA_HOME/bin/javap -p -l -v -s -c out/java_util_Date_loaded.class > out/java_util_Date_loaded.class.disasm.txt
$JAVA_HOME/bin/javap -p -l -v -s -c out/java_util_Date_redefined.class > out/java_util_Date_redefined.class.disasm.txt

set +e
FOUND_STACKMAP=`grep "StackMapTable: number_of_entries" out/java_util_Date_redefined.class.disasm.txt`
set -e

if [ -z "$FOUND_STACKMAP" ]; then
	echo "Test failed: StackMapTable not found in redefined bytes."
	exit 1
fi

echo "Test passed."
