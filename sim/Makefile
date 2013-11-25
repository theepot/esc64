VVP						:= computer.vvp
VVP_ABSTRACT			:= computer_abstract.vvp
THRIFT_VPI_DIR			:= thrift-server
THRIFT_VPI				:= ${THRIFT_VPI_DIR}/thriftserver.vpi
V_SOURCE				:= common-modules/computer.v
V_DEPENDENCIES_ABSTRACT := $(wildcard abstract-modules/*.v) $(wildcard common-modules/*.v)
V_DEPENDENCIES_DISCRETE := $(wildcard discrete-modules/*.v) $(wildcard common-modules/*.v) $(wildcard ICs/*.v)

.PHONY: all
all: ${VVP} ${THRIFT_VPI}

.PHONY: clean
clean:
	rm -f ${VVP}
	rm -f ${VPP_ABSTRACT}
	${MAKE} -C ${THRIFT_VPI_DIR} clean

${VVP}: ${V_SOURCE} ${V_DEPENDENCIES_DISCRETE}
	iverilog -DALL_STRUCT -Icommon-modules -Idiscrete-modules -IICs -o$@ $<

${VVP_ABSTRACT}: ${V_SOURCE} ${V_DEPENDENCIES_ABSTRACT}
	iverilog -DALL_STRUCT -Icommon-modules -Iabstract-modules -o$@ $<

.PHONY: ${THRIFT_VPI}
${THRIFT_VPI}:
	${MAKE} -C ${THRIFT_VPI_DIR}

