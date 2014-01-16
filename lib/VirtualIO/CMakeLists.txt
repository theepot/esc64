cmake_minimum_required(VERSION 2.6)

set(SOURCE_DIR src)

find_package(Memory REQUIRED PATHS ../Memory NO_DEFAULT_PATH)

set(LIB_SOURCES
	${SOURCE_DIR}/RAM.cpp
	${SOURCE_DIR}/VirtualIO.cpp
	)

include_directories(include)
include_directories(${Memory_INCLUDE_DIRS})

set(LIBRARY_OUTPUT_PATH lib)

add_library(VirtualIO STATIC ${LIB_SOURCES})

set_target_properties(VirtualIO PROPERTIES COMPILE_FLAGS "-Wall -fPIC -g3")