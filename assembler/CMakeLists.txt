cmake_minimum_required(VERSION 2.6)
project(esc-assembler)

set(CMAKE_C_FLAGS "-Wall")
set(CMAKE_C_FLAGS_DEBUG "-O0 -g3 -DESC_DEBUG -DESC_TEST")
set(CMAKE_C_FLAGS_RELEASE "-O2")
set(CMAKE_SHARED_LINKER_FLAGS "-Wall")

set(CMAKE_BUILD_TYPE DEBUG)

set(KEYWORDS_INL include/esc64asm/keywords_gen.h)
set(KEYWORDS_GPERF keywords.gperf)
set(KW_GEN_PATH kwgen)
set(KW_GEN_DESCS ${KW_GEN_PATH}/descriptions.rb)
set(KW_GEN_FILES ${KW_GEN_PATH}/interpreter.rb ${KW_GEN_DESCS})
set(DECOMP_INL include/esc64asm/decomp_gen.h)

#set(SOURCE_DIR ${CMAKE_BINARY_DIR}/src)
set(SOURCE_DIR src)
set(TEST_DIR ${SOURCE_DIR}/tests)
#set(LIB_DIR lib)
get_filename_component(TESTFILES "testfiles" ABSOLUTE)
get_filename_component(LIB_DIR "lib" ABSOLUTE)
get_filename_component(EXECUTABLE_OUTPUT_PATH "bin" ABSOLUTE)

set(
	COMMON_SOURCES
	${SOURCE_DIR}/align.c
	${SOURCE_DIR}/hashset.c
	${SOURCE_DIR}/ioutils.c
	${SOURCE_DIR}/link.c
	${SOURCE_DIR}/objread.c
	${SOURCE_DIR}/objwrite.c
	${SOURCE_DIR}/objrecord.c
	${SOURCE_DIR}/parser.c
	${SOURCE_DIR}/scanner.c
	${SOURCE_DIR}/symtable.c
	${SOURCE_DIR}/token.c
	${SOURCE_DIR}/freelist.c
	${SOURCE_DIR}/escerror.c
	${SOURCE_DIR}/opcodes.c
	${SOURCE_DIR}/mempool.c
	${SOURCE_DIR}/keywords.c
	${SOURCE_DIR}/decomp.c)

set(
	AS_SOURCES
	${SOURCE_DIR}/as.c)

set(
	LN_SOURCES
	${SOURCE_DIR}/ln.c)

set(
	OBJDUMP_SOURCES
	${SOURCE_DIR}/objdump.c)

set(
	UNIT_TEST_SOURCES
	${TEST_DIR}/exec_write_read_test.c
	${TEST_DIR}/main.c
	${TEST_DIR}/objfile_test.c
	${TEST_DIR}/parser_test.c
	${TEST_DIR}/record_test.c
	${TEST_DIR}/reswords_test.c
	${TEST_DIR}/scanner_test.c
	${TEST_DIR}/symtable_test.c
	${TEST_DIR}/freelist_test.c)

set(
	EXEDUMP_SOURCES
	${SOURCE_DIR}/exedump.c)

set(
	EXETOLST_SOURCES
	${SOURCE_DIR}/exetolst.c)
	
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIB_DIR})
#set(EXECUTABLE_OUTPUT_PATH bin)

include_directories(include)
link_directories(${LIB_DIR})

##### keywords #####
add_custom_command(	OUTPUT ${KEYWORDS_INL}
					COMMAND gperf --enum --omit-struct-type --language=C --lookup-function-name=FindKeyword_ --hash-function-name=Hash --slot-name=keyword -C -t --ignore-case ${KEYWORDS_GPERF} | sed -f comment_line_cmds.sed > ${KEYWORDS_INL}
					DEPENDS ${KEYWORDS_GPERF}
					COMMENT "Generating ${KEYWORDS_INL}")
	
add_custom_command(	OUTPUT ${KEYWORDS_GPERF}
					COMMAND ruby -I ${KW_GEN_PATH} ${KW_GEN_DESCS} -g ${KEYWORDS_GPERF}
					DEPENDS ${KW_GEN_FILES}
					COMMENT "Generating ${KEYWORDS_GPERF}")
					
add_custom_target(genkeywords DEPENDS ${KEYWORDS_INL})

##### decompilation #####
add_custom_command(	OUTPUT ${DECOMP_INL}
					COMMAND ruby -I ${KW_GEN_PATH} ${KW_GEN_DESCS} -d ${DECOMP_INL}
					DEPENDS ${KW_GEN_FILES}
					COMMENT "Generating ${DECOMP_INL}")
					
add_custom_target(gendecomp DEPENDS ${DECOMP_INL})

##### common SO #####
add_definitions(-DESC_TEST_FILE_BASE_PATH="${TESTFILES}")
add_library(esc-asm-common SHARED ${COMMON_SOURCES})
add_dependencies(esc-asm-common gendecomp)
add_dependencies(esc-asm-common genkeywords)

##### executables #####
add_executable(esc-as ${AS_SOURCES})
target_link_libraries(esc-as esc-asm-common)

add_executable(esc-ln ${LN_SOURCES})
target_link_libraries(esc-ln esc-asm-common)

add_executable(esc-objdump ${OBJDUMP_SOURCES})
target_link_libraries(esc-objdump esc-asm-common)

add_executable(esc-unit-tests ${UNIT_TEST_SOURCES})
target_link_libraries(esc-unit-tests esc-asm-common)

add_executable(esc-exedump ${EXEDUMP_SOURCES})
target_link_libraries(esc-exedump esc-asm-common)

add_executable(esc-exetolst ${EXETOLST_SOURCES})
target_link_libraries(esc-exetolst esc-asm-common)
