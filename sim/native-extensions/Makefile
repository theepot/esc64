.SUFFIXES:

# compiler / linker flags
CPPFLAGS			=-DHAVE_INTTYPES_H -DHAVE_NETINET_IN_H
CXXFLAGS			=-fPIC -Wall -g3 -O0 -fstack-protector --param=ssp-buffer-size=4 -Wformat -Werror=format-security -I/usr/include/iverilog -I/usr/local/include/thrift -Isrc -I../../lib/Memory/include -I../../lib/VirtualIO/include
LDFLAGS				= -Wall -shared -L/usr/local/lib -lthrift -lthriftnb -lboost_thread -lboost_program_options -levent

SRC					=src
SERVICE_GEN_DIR		=${SRC}/service
SERVICE				=../service.thrift
SERVICE_TIMESTAMP	=service_timestamp
SERVICE_GEN_SRC		=${SERVICE_GEN_DIR}/service_constants.cpp ${SERVICE_GEN_DIR}/service_constants.h ${SERVICE_GEN_DIR}/service_types.cpp ${SERVICE_GEN_DIR}/service_types.h ${SERVICE_GEN_DIR}/SimService.cpp ${SERVICE_GEN_DIR}/SimService.h ${SERVICE_GEN_DIR}/SimService_server.skeleton.cpp
OBJS				=SimService.o service_constants.o service_types.o SimControl.o VpiUtils.o VirtualIO.o
HEADERS				=$(wildcard ${SRC}/*.hpp)
TARGET				=native-extensions.vpi

# rules
.PHONY: all
all: ${TARGET}

.PHONY: gen
gen: ${SERVICE_GEN_DIR}

.PHONY: clean
clean:
	${RM} ${SERVICE_GEN_SRC}
	rmdir --ignore-fail-on-non-empty ${SERVICE_GEN_DIR}
	${RM} ${OBJS} ${TARGET}
	${RM} ${SERVICE_TIMESTAMP}

.PHONY: ../../lib/Memory/lib/libMemory.a
../../lib/Memory/lib/libMemory.a:
	$(MAKE) -C ../../lib/Memory

.PHONY: ../../lib/VirtualIO/lib/libVirtualIO.a
../../lib/VirtualIO/lib/libVirtualIO.a:
	$(MAKE) -C ../../lib/VirtualIO

${SERVICE_TIMESTAMP}: ${SERVICE}
	mkdir -p ${SERVICE_GEN_DIR}
	thrift -out ${SERVICE_GEN_DIR} --gen cpp ${SERVICE}
	touch $@
	
${SERVICE_GEN_SRC}: ${SERVICE_TIMESTAMP}

CXX_CMD =${CXX} ${CPPFLAGS} ${CXXFLAGS} -c -o $@ $< ${LDFLAGS}

%.o: ${SRC}/%.cpp ${HEADERS}
	${CXX_CMD}

%.o: ${SERVICE_GEN_DIR}/%.cpp
	${CXX_CMD}

${TARGET}: ${OBJS}  ../../lib/Memory/lib/libMemory.a ../../lib/VirtualIO/lib/libVirtualIO.a
	${CXX} -Wl,--whole-archive ../../lib/Memory/lib/libMemory.a ../../lib/VirtualIO/lib/libVirtualIO.a -Wl,--no-whole-archive -o $@ $^ ${LDFLAGS}
