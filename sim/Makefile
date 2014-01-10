VVP						:= computer.vvp
VVP_ABSTRACT			:= computer_abstract.vvp
NATIVE_EXT_DIR			:= native-extensions
NATIVE_EXT_VPI			:= ${NATIVE_EXT_DIR}/native-extensions.vpi
V_SOURCE				:= common-modules/computer.v
V_DEPENDENCIES_ABSTRACT := $(wildcard abstract-modules/*.v) $(wildcard common-modules/*.v)
V_DEPENDENCIES_DISCRETE := $(wildcard discrete-modules/*.v) $(wildcard common-modules/*.v) $(wildcard ICs/*.v)

.PHONY: all
all: ${VVP} ${NATIVE_EXT_VPI}

.PHONY: clean
clean:
	rm -f ${VVP}
	rm -f ${VPP_ABSTRACT}
	${MAKE} -C ${NATIVE_EXT_DIR} clean

${VVP}: ${V_SOURCE} ${V_DEPENDENCIES_DISCRETE}
	iverilog -DALL_STRUCT -Icommon-modules -Idiscrete-modules -IICs -o$@ $<

${VVP_ABSTRACT}: ${V_SOURCE} ${V_DEPENDENCIES_ABSTRACT}
	iverilog -DALL_STRUCT -Icommon-modules -Iabstract-modules -o$@ $<

.PHONY: ${NATIVE_EXT_VPI}
${NATIVE_EXT_VPI}:
	${MAKE} -C ${NATIVE_EXT_DIR}

