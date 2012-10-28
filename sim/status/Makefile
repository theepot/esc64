.PHONY: all
all: status.test status_s.test

status.test: status.v status_test.v ../register/register.v
	iverilog -Wall -o status.test status_test.v

status_s.test: status_s.v status_test.v ../74xxx/octRegister_74377.v
	iverilog -Wall -o status_s.test -DSTATUS_STRUCT status_test.v
