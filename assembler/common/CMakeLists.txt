cmake_minimum_required(VERSION 2.6)
project(Esc64AsmCommon)

set(CMAKE_C_FLAGS "-Wall")
set(CMAKE_C_FLAGS_DEBUG "-O0 -g3 -DESC_DEBUG -DESC_TEST")
set(CMAKE_C_FLAGS_RELEASE "-O2")
set(CMAKE_SHARED_LINKER_FLAGS "-Wall")

set(CMAKE_BUILD_TYPE DEBUG)

set(	SOURCES
	${CMAKE_BINARY_DIR}/src/hashset.c
	${CMAKE_BINARY_DIR}/src/ioutils.c
	${CMAKE_BINARY_DIR}/src/link.c
	${CMAKE_BINARY_DIR}/src/map.c
	${CMAKE_BINARY_DIR}/src/objcode.c
	${CMAKE_BINARY_DIR}/src/objrecord.c
	${CMAKE_BINARY_DIR}/src/parser.c
	${CMAKE_BINARY_DIR}/src/reswords.c
	${CMAKE_BINARY_DIR}/src/scanner.c
	${CMAKE_BINARY_DIR}/src/symtable.c
	${CMAKE_BINARY_DIR}/src/tokendescr.c)

include_directories(${CMAKE_BINARY_DIR}/include)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_library(Esc64AsmCommon SHARED ${SOURCES})

