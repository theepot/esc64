cmake_minimum_required(VERSION 2.6)
project(Esc64ExeDump)

set(CMAKE_C_FLAGS "-Wall")
set(CMAKE_C_FLAGS_DEBUG "-O0 -g3 -DESC_DEBUG")
set(CMAKE_C_FLAGS_RELEASE "-O2")

set(CMAKE_BUILD_TYPE DEBUG)

set(	SOURCES
	${CMAKE_BINARY_DIR}/src/main.c)

include_directories(${CMAKE_BINARY_DIR}/../common/include)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

link_directories(${CMAKE_BINARY_DIR}/../common/lib)

add_executable(esc-exedump ${SOURCES})
target_link_libraries(esc-exedump Esc64AsmCommon)

add_custom_command(
	TARGET esc-exedump
	PRE_BUILD
	COMMAND make
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/../common)
