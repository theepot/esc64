cmake_minimum_required(VERSION 2.6)
project(Esc64As)

set(CMAKE_C_FLAGS "-Wall")
set(CMAKE_C_FLAGS_DEBUG "-O0 -g3 -DESC_DEBUG -DESC_TEST")
set(CMAKE_C_FLAGS_RELEASE "-O2")

add_definitions(-DESC_TEST_FILE_BASE_PATH="${CMAKE_BINARY_DIR}/../testfiles")

set(CMAKE_BUILD_TYPE DEBUG)

set(	SOURCES
	${CMAKE_BINARY_DIR}/src/exec_write_read_test.c
	${CMAKE_BINARY_DIR}/src/main.c
	${CMAKE_BINARY_DIR}/src/objfile_test.c
	${CMAKE_BINARY_DIR}/src/parser_test.c
	${CMAKE_BINARY_DIR}/src/record_test.c
	${CMAKE_BINARY_DIR}/src/reswords_test.c
	${CMAKE_BINARY_DIR}/src/scanner_test.c
	${CMAKE_BINARY_DIR}/src/symtable_test.c)

include_directories(${CMAKE_BINARY_DIR}/../common/include)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

link_directories(${CMAKE_BINARY_DIR}/../common/lib)

add_executable(esc-unit-tests ${SOURCES})
target_link_libraries(esc-unit-tests Esc64AsmCommon)

add_custom_command(
	TARGET esc-unit-tests
	PRE_BUILD
	COMMAND make
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/../common)
