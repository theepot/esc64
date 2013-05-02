GLOBALS			:= ../globals.v
COMPONENTS		:= $(wildcard ../*/*.v)
VVP				:= computer.vpp
THRIFT_VPI_DIR	:= thrift-server
THRIFT_VPI		:= ${THRIFT_VPI_DIR}/thriftserver.vpi
SOURCES			:= computer.v ${COMPONENTS} ${GLOBALS}

.PHONY: all
all: ${VVP} ${THRIFT_VPI}

.PHONY: clean
clean:
	rm -f ${VVP}
	${MAKE} -C ${THRIFT_VPI_DIR} clean

computer.vpp: ${SOURCES}
	iverilog -DALL_STRUCT -o$@ $^

.PHONY: ${THRIFT_VPI}
${THRIFT_VPI}:
	${MAKE} -C ${THRIFT_VPI_DIR}

