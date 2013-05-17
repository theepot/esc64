.SUFFIXES:

# compiler / linker flags
CPPFLAGS			=-DHAVE_INTTYPES_H -DHAVE_NETINET_IN_H
CXXFLAGS			=-fPIC -Wall -g3 -O0 -fstack-protector --param=ssp-buffer-size=4 -Wformat -Werror=format-security -I/usr/include/iverilog -I/usr/local/include/thrift
LDFLAGS				=-shared -L/usr/local/lib -lthrift -lboost_thread

SRC					=src
SERVICE_GEN_DIR		=${SRC}/service
SERVICE				=service.thrift
SERVICE_TIMESTAMP	=service_timestamp
SERVICE_GEN_SRC		=${SERVICE_GEN_DIR}/Service.cpp ${SERVICE_GEN_DIR}/Service_server.skeleton.cpp ${SERVICE_GEN_DIR}/service_constants.cpp ${SERVICE_GEN_DIR}/service_types.cpp
OBJS				=Service.o service_constants.o service_types.o Server.o
TARGET				=thriftserver.vpi

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

${SERVICE_TIMESTAMP}: ${SERVICE}
	mkdir -p ${SERVICE_GEN_DIR}
	thrift -out ${SERVICE_GEN_DIR} --gen cpp ${SERVICE}
	touch $@
	
${SERVICE_GEN_SRC}: ${SERVICE_TIMESTAMP}

CXX_CMD =${CXX} ${CPPFLAGS} ${CXXFLAGS} -c -o $@ $< ${LDFLAGS}

%.o: ${SRC}/%.cpp
	${CXX_CMD}

%.o: ${SERVICE_GEN_DIR}/%.cpp
	${CXX_CMD}

${TARGET}: ${OBJS}
	${CXX} -o $@ $^ ${LDFLAGS}
