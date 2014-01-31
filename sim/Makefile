VVP						:= computer.vvp
NATIVE_EXT_DIR			:= native-extensions
NATIVE_EXT_VPI			:= ${NATIVE_EXT_DIR}/native-extensions.vpi
V_SOURCE				:= modules/computer.v
V_DEPENDENCIES			:= $(wildcard modules/*.v) $(wildcard ICs/*.v)

.PHONY: all
all: ${VVP} ${NATIVE_EXT_VPI}

.PHONY: clean
clean:
	rm -f ${VVP}
	${MAKE} -C ${NATIVE_EXT_DIR} clean

${VVP}: ${V_SOURCE} ${V_DEPENDENCIES}
	iverilog -DALL_STRUCT -Imodules -IICs -o$@ $<

.PHONY: ${NATIVE_EXT_VPI}
${NATIVE_EXT_VPI}:
	${MAKE} -C ${NATIVE_EXT_DIR}

