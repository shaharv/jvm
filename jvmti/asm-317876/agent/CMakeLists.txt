cmake_minimum_required (VERSION 3.6)

project (AgentTest)

add_subdirectory (src)

include_directories($ENV{JAVA_HOME}/include)
if (UNIX)
        include_directories($ENV{JAVA_HOME}/include/linux)
endif ()

add_library(agent SHARED src/Agent.cpp)
