.SUFFIXES:

# compiler / linker flags
CPPFLAGS		:= -DHAVE_INTTYPES_H -DHAVE_NETINET_IN_H
CXXFLAGS		:= -fPIC -Wall -g3 -O0 -fstack-protector --param=ssp-buffer-size=4 -Wformat -Werror=format-security -I/usr/include/iverilog -I/usr/local/include/thrift
LDFLAGS			:= -shared -L/usr/local/lib -lthrift -lboost_thread

SRC				:= src
#COMMON			:= ../common
HELLO_GEN_DIR	:= ${SRC}/hello-gen
HELLO_SERVICE	:= service.thrift
HELLO_GEN_SRC	:= ${HELLO_GEN_DIR}/HelloService.cpp ${HELLO_GEN_DIR}/HelloService_server.skeleton.cpp ${HELLO_GEN_DIR}/service_constants.cpp ${HELLO_GEN_DIR}/service_types.cpp
OBJS			:= HelloService.o service_constants.o service_types.o Server.o
TARGET			:= thriftserver.vpi

# rules
.PHONY: all
all: ${TARGET}

.PHONY: gen
gen: ${HELLO_GEN_DIR}

.PHONY: clean
clean:
	${RM} -r ${HELLO_GEN_DIR}
	${RM} ${OBJS} ${TARGET}

${HELLO_GEN_DIR}: ${HELLO_SERVICE}
	mkdir -p ${HELLO_GEN_DIR}
	thrift -out ${HELLO_GEN_DIR} --gen cpp ${HELLO_SERVICE}

${HELLO_GEN_SRC}: ${HELLO_GEN_DIR}

CXX_CMD = ${CXX} ${CPPFLAGS} ${CXXFLAGS} -c -o $@ $< ${LDFLAGS}

%.o: ${SRC}/%.cpp
	${CXX_CMD}

%.o: ${HELLO_GEN_DIR}/%.cpp
	${CXX_CMD}

${TARGET}: ${OBJS}
	${CXX} -o $@ $^ ${LDFLAGS}
