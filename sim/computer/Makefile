GLOBALS			:= ../globals.v
COMPONENTS		:= $(wildcard ../*/*.v)
VVP				:= computer.vpp
DBUS_VPI_DIR	:= dbus-server
DBUS_VPI		:= ${DBUS_VPI_DIR}/sim-dbus-server.vpi
SOURCES			:= computer.v ${COMPONENTS} ${GLOBALS}

.PHONY: all
all: ${VVP} ${DBUS_VPI}

.PHONY: clean
clean:
	rm -f ${VVP}
	${MAKE} -C ${DBUS_VPI_DIR} clean

computer.vpp: ${SOURCES}
	iverilog -DALL_STRUCT -o$@ $^

#TODO put this in a bash script
#computer.vcd: computer.vvp urom.lst ram.lst interactive.vpi
#	vvp -M<vpi search path> -m<vpi's to load -.vpi> <vpp>

.PHONY: ${DBUS_VPI}
${DBUS_VPI}:
	${MAKE} -C ${DBUS_VPI_DIR}

