cmake_minimum_required (VERSION 3.6)

project (AgentTest)

add_subdirectory (src)

include_directories($ENV{JAVA_HOME}/include)
if (UNIX)
        include_directories($ENV{JAVA_HOME}/include/linux)
endif ()

if (CMAKE_SYSTEM_NAME STREQUAL "AIX")
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -q64")
else ()
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif ()

add_library(agent SHARED src/Agent.cpp)
