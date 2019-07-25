# asm-317876

Simple JVMTI agent reproducer for the JVM issue reported in:  
https://gitlab.ow2.org/asm/asm/issues/317876

The resulting JDK ticket:   
https://bugs.java.com/bugdatabase/view_bug.do?bug_id=JDK-8228604

### Test requirements

The following programs are required for running the JVMTI agent test:

- JDK
- cmake
- make
- A C++ compiler

Alpine one liner:  
`apk add bash cmake g++ make openjdk8`

Ubuntu one liner:  
`apt-get install cmake g++ make openjdk-8-jdk`

### Running the test

On Linux, run `./test.sh`.

This will:
1. Build the sample JVMTI agent
2. Build the Java sample program
3. Start java with the sample JVMTI agent and the sample program
4. Disassemble the test class bytes, dumped on first class load and after retransformation
5. Grep the disassembly to check the presense of `StackMapTable`.

Note that `JAVA_HOME` must be set to the desired JDK location.
