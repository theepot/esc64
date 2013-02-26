cmake_minimum_required(VERSION 2.6)
project(esc-assembler)

set(CMAKE_C_FLAGS "-Wall")
set(CMAKE_C_FLAGS_DEBUG "-O0 -g3 -DESC_DEBUG -DESC_TEST")
set(CMAKE_C_FLAGS_RELEASE "-O2")
set(CMAKE_SHARED_LINKER_FLAGS "-Wall")

set(CMAKE_BUILD_TYPE DEBUG)

set(SOURCE_DIR ${CMAKE_BINARY_DIR}/src)
set(TEST_DIR ${SOURCE_DIR}/tests)
set(LIB_DIR ${CMAKE_BINARY_DIR}/lib)

set(
	COMMON_SOURCES
	${SOURCE_DIR}/hashset.c
	${SOURCE_DIR}/ioutils.c
	${SOURCE_DIR}/link.c
	${SOURCE_DIR}/map.c
	${SOURCE_DIR}/objcode.c
	${SOURCE_DIR}/objrecord.c
	${SOURCE_DIR}/parser.c
	${SOURCE_DIR}/reswords.c
	${SOURCE_DIR}/scanner.c
	${SOURCE_DIR}/symtable.c
	${SOURCE_DIR}/tokendescr.c)

set(
	AS_SOURCES
	${SOURCE_DIR}/as.c)

set(
	LN_SOURCES
	${SOURCE_DIR}/ln.c)

set(
	OBJDUMP_SOURCES
	${SOURCE_DIR}/objdump.c
	${SOURCE_DIR}/opcodetrans.c)

set(
	UNIT_TEST_SOURCES
	${TEST_DIR}/exec_write_read_test.c
	${TEST_DIR}/main.c
	${TEST_DIR}/objfile_test.c
	${TEST_DIR}/parser_test.c
	${TEST_DIR}/record_test.c
	${TEST_DIR}/reswords_test.c
	${TEST_DIR}/scanner_test.c
	${TEST_DIR}/symtable_test.c)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIB_DIR})
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

include_directories(${CMAKE_BINARY_DIR}/include)
link_directories(${LIB_DIR})

add_definitions(-DESC_TEST_FILE_BASE_PATH="${CMAKE_BINARY_DIR}/testfiles")
add_library(esc-asm-common SHARED ${COMMON_SOURCES})

add_executable(esc-as ${AS_SOURCES})
target_link_libraries(esc-as esc-asm-common)

add_executable(esc-ln ${LN_SOURCES})
target_link_libraries(esc-ln esc-asm-common)

add_executable(esc-objdump ${OBJDUMP_SOURCES})
target_link_libraries(esc-objdump esc-asm-common)

add_executable(esc-unit-tests ${UNIT_TEST_SOURCES})
target_link_libraries(esc-unit-tests esc-asm-common)

