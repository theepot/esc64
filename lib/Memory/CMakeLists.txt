cmake_minimum_required(VERSION 2.6)

find_package(FLEX)

set(SOURCE_DIR src)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

FLEX_TARGET(VerilogMemScanner ${SOURCE_DIR}/verilogmem.lex ${SOURCE_DIR}/verilogmem.lex.c COMPILE_FLAGS -L)

set(TEST_SOURCES ${SOURCE_DIR}/VerilogmemTest.cpp ${FLEX_VerilogMemScanner_OUTPUTS})

set(LIB_SOURCES
#	${SOURCE_DIR}/Memory.cpp
#	${SOURCE_DIR}/VerilogMemoryFileLoader.cpp
	${FLEX_VerilogMemScanner_OUTPUTS}
	)

include_directories(include)
set(LIBRARY_OUTPUT_PATH lib)

add_library(Memory STATIC ${LIB_SOURCES})
add_executable(verilogMemoryTest ${TEST_SOURCES}) 
