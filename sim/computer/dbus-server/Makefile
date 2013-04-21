.SUFFIXES:

PKG_CONF_ARGS		:= dbus-c++-1
VPI_CFLAGS			:= $(shell iverilog-vpi --cflags)
VPI_LDFLAGS			:= $(shell iverilog-vpi --ldflags)
CXXFLAGS			:=
LDFLAGS				:=
VPI					:= sim-dbus-server.vpi
SRC					:= src
DBUS_SERVICE		:= service.xml
DBUS_ADAPTOR_GEN	:= ${SRC}/sim-dbus-adaptor.hpp
DBUS_ADAPTOR_IMPL	:= ${SRC}/sim-dbus-server.cpp
OBJECTS				:= sim-dbus-server.o

CXXFLAGS	+= ${VPI_CFLAGS} $(shell pkg-config --cflags ${PKG_CONF_ARGS})
LDFLAGS		+= ${VPI_LDFLAGS} $(shell pkg-config --libs ${PKG_CONF_ARGS})

.PHONY: all
all: ${VPI}

.PHONY: clean
clean:
	rm -f ${VPI} ${OBJECTS} ${DBUS_ADAPTOR_GEN}

${DBUS_ADAPTOR_GEN}: ${DBUS_SERVICE}
	dbusxx-xml2cpp $< --adaptor=$@

${DBUS_ADAPTOR_IMPL}: ${DBUS_ADAPTOR_GEN}

%.o: ${SRC}/%.cpp
	${CXX} ${CXXFLAGS} -c -o $@ $< ${LDFLAGS}

${VPI}: ${OBJECTS}
	${CXX} -o $@ $^ ${LDFLAGS}
