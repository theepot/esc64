cmake_minimum_required(VERSION 2.6)

find_package(VirtualIO REQUIRED PATHS ../lib/VirtualIO NO_DEFAULT_PATH)
find_package(CPU REQUIRED PATHS ../lib/cpu NO_DEFAULT_PATH)
find_package(Boost REQUIRED)
find_package(ComputerControlService REQUIRED PATHS ../lib/ComputerControlService NO_DEFAULT_PATH)
find_package(CInstrInfo REQUIRED PATHS ../lib/CInstrInfo NO_DEFAULT_PATH)

include_directories(${VirtualIO_INCLUDE_DIRS})
include_directories(${CPU_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
include_directories(src)
include_directories(/usr/local/include/thrift)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g3 -DHAVE_INTTYPES_H -DHAVE_NETINET_IN_H")
set(THRIFT_GEN_DIR src/service)
set(THRIFT_GEN_TIMESTAMP thrift_gen_timestamp)

set(THRIFT_GEN_SOURCES 
	${THRIFT_GEN_DIR}/service_constants.cpp
	${THRIFT_GEN_DIR}/service_constants.h
	${THRIFT_GEN_DIR}/service_types.cpp
	${THRIFT_GEN_DIR}/service_types.h
	${THRIFT_GEN_DIR}/ComputerControlService.cpp
	${THRIFT_GEN_DIR}/ComputerControlService.h
	)

add_custom_command(	OUTPUT ${THRIFT_GEN_SOURCES}
					COMMAND mkdir -p ${THRIFT_GEN_DIR}
					COMMAND thrift -out ${THRIFT_GEN_DIR} --gen cpp ${COMPUTER_CONTROL_SERVICE_THRIFT_FILE}   
					DEPENDS ${COMPUTER_CONTROL_SERVICE_THRIFT_FILE}
					COMMENT "Generating ${THRIFT_GEN_DIR} from ${COMPUTER_CONTROL_SERVICE_THRIFT_FILE}")

set_property(DIRECTORY "${CMAKE_BINARY_DIR}" APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES ${THRIFT_GEN_SOURCES})

include_directories(${THRIFT_GEN_DIR})
include_directories(${CInstrInfo_INCLUDE_DIRS})

add_custom_target(genThriftService DEPENDS ${THRIFT_GEN_SOURCES})

set(ESC64VM_SOURCES
	src/main.cpp
	src/ESC64.cpp
	${THRIFT_GEN_SOURCES}
	${CInstrInfo_SOURCE_FILES}
)

add_executable(esc64vm ${ESC64VM_SOURCES})
target_link_libraries(esc64vm ${VirtualIO_LIBRARY})
target_link_libraries(esc64vm thrift)
target_link_libraries(esc64vm thriftnb)
target_link_libraries(esc64vm event)
target_link_libraries(esc64vm boost_thread)
target_link_libraries(esc64vm boost_system)
