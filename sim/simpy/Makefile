SERVICE_GEN_DIR		=gen-py
SERVICE				=../service.thrift
SERVICE_TIMESTAMP	=service_timestamp

.PHONY: all
all: ${SERVICE_TIMESTAMP}

.PHONY: clean
clean:
	${RM} ${SERVICE_GEN_SRC}
	${RM} -r ${SERVICE_GEN_DIR}
	${RM} ${SERVICE_TIMESTAMP}

${SERVICE_TIMESTAMP}: ${SERVICE}
	mkdir -p ${SERVICE_GEN_DIR}
	thrift -out ${SERVICE_GEN_DIR} --gen py ${SERVICE}
	touch $@

