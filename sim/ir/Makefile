.PHONY: all
all: ir.test ir_s.test

ir.test: ir_test.v ir.v ../register/register.v ../globals.v
	iverilog -Wall -o ir.test ir_test.v

ir_s.test: ir_test.v ir_s.v ../74xxx/octRegister_74377.v ../globals.v
	iverilog -Wall -o ir_s.test -DIR_STRUCT ir_test.v
