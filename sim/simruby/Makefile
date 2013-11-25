
SERVICE_GEN_DIR		=service
SERVICE				=../thrift-server/service.thrift
SERVICE_TIMESTAMP	=service_timestamp

.PHONY: all
all: ${SERVICE_TIMESTAMP}

.PHONY: clean
clean:
	${RM} ${SERVICE_GEN_SRC}
	rmdir --ignore-fail-on-non-empty ${SERVICE_GEN_DIR}
	${RM} ${SERVICE_TIMESTAMP}

${SERVICE_TIMESTAMP}: ${SERVICE}
	mkdir -p ${SERVICE_GEN_DIR}
	thrift -out ${SERVICE_GEN_DIR} --gen rb ${SERVICE}
	touch $@

