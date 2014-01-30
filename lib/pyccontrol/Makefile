SERVICE_GEN_DIR		=gen-py
SERVICE				=../ComputerControlService/service.thrift
SERVICE_TIMESTAMP	=service_timestamp
DISASM_INFO			=gen_disasm.py
INFOGEN				=../../infogen

.PHONY: all
all: ${SERVICE_TIMESTAMP} ${DISASM_INFO}

.PHONY: clean
clean:
	${RM} ${SERVICE_GEN_SRC}
	${RM} -r ${SERVICE_GEN_DIR}
	${RM} ${SERVICE_TIMESTAMP}

${SERVICE_TIMESTAMP}: ${SERVICE}
	mkdir -p ${SERVICE_GEN_DIR}
	thrift -out ${SERVICE_GEN_DIR} --gen py ${SERVICE}
	touch $@

${DISASM_INFO}: ${INFOGEN}/interpreter.rb ${INFOGEN}/descriptions.rb
	ruby -I${INFOGEN} ${INFOGEN}/descriptions.rb -y $@
