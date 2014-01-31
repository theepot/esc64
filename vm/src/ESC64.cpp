#include <cpu.h>
#include <instr_info.h>
#include <ESC64.hpp>
#include <cassert>
#include <cstdio>

using namespace ::virtual_io;

ESC64::ESC64(VirtualIOManager* viom) : viom(viom) {
	reset();
}

ESC64::~ESC64() {
}

void ESC64::step(void) {
	validate_some_stuff();

	if(state != OK)
		return;
	Instr i;
	if(fetch(&i)) {
		execute(i);
		if(i.opcode != op_halt) {
			step_count++;
		}
	}
}

void ESC64::reset(void) {
	for(int i = 0; i < 8; ++i) {
		regs[i] = 0;
	}
	state = OK;
	c_flag = false;
	c_flag_is_defined = true;
	z_flag = false;
	step_count = 0;
}

bool ESC64::fetch(ESC64::Instr* out_instr) {
	int read_data;
	if(!safe_read_word(regs[RGS_PC], false, &read_data)) {
		return false;
	}

	Instr result;
	result.opcode = (opcode_t)((read_data >> 9) & 0x01FF);
	if(instr_info[result.opcode].opcode == 0) {
		state = UNKNOWN_OPCODE;
		fprintf(stderr, "esc64vm: WARNING: encounterd unknown opcode %d\n", result.opcode);
		return false;
	}
	result.op0 = (read_data >> 6) & 0x7;
	result.op1 = (read_data >> 3) & 0x7;
	result.op2 = (read_data >> 0) & 0x7;

	pc_next_word();

	if(instr_info[result.opcode].wide) {
		if(!safe_read_word(regs[RGS_PC], false, &read_data)) {
			return false;
		}
		result.op3 = read_data;

		pc_next_word();
	}

	*out_instr = result;
	return true;
}

// (op[^ \t]+).+$
// case \1:\n\t{\n\t}\n\tbreak;
void ESC64::execute(Instr i) {
	switch(i.opcode) {
	case op_add:
		regs[i.op0] = regs[i.op1] + regs[i.op2];
		z_flag = (regs[i.op0] & 0xFFFF) == 0;
		c_flag = regs[i.op0] > 0xFFFF;
		c_flag_is_defined = true;
		break;
	case op_adc:
		if(!c_flag_is_defined) {
			state = UNDEFINED_EFFECT;
			break;
		}
		regs[i.op0] = regs[i.op1] + regs[i.op2] + (c_flag ? 1 : 0);
		z_flag = (regs[i.op0] & 0xFFFF) == 0;
		c_flag = regs[i.op0] > 0xFFFF;
		c_flag_is_defined = true;
		break;
	case op_sub:
		regs[i.op0] = regs[i.op1] - regs[i.op2];
		z_flag = (regs[i.op0] & 0xFFFF) == 0;
		c_flag = regs[i.op0] <= 0xFFFF;
		c_flag_is_defined = true;
		break;
	case op_sbc:
		if(!c_flag_is_defined) {
			state = UNDEFINED_EFFECT;
			break;
		}
		regs[i.op0] = regs[i.op1] - regs[i.op2] - 1 + (c_flag ? 1 : 0);
		z_flag = (regs[i.op0] & 0xFFFF) == 0;
		c_flag = regs[i.op0] <= 0xFFFF;
		c_flag_is_defined = true;
		break;
	case op_inc:
		regs[i.op0] = regs[i.op1] + 1;
		z_flag = (regs[i.op0] & 0xFFFF) == 0;
		c_flag = regs[i.op0] > 0xFFFF;
		c_flag_is_defined = true;
		break;
	case op_dec:
		regs[i.op0] = regs[i.op1] - 1;
		z_flag = (regs[i.op0] & 0xFFFF) == 0;
		c_flag = regs[i.op0] <= 0xFFFF;
		c_flag_is_defined = true;
		break;
	case op_or:
		regs[i.op0] = regs[i.op1] | regs[i.op2];
		z_flag = (regs[i.op0] & 0xFFFF) == 0;
		c_flag_is_defined = false;
		break;
	case op_xor:
		regs[i.op0] = regs[i.op1] ^ regs[i.op2];
		z_flag = (regs[i.op0] & 0xFFFF) == 0;
		c_flag_is_defined = false;
		break;
	case op_and:
		regs[i.op0] = regs[i.op1] & regs[i.op2];
		z_flag = (regs[i.op0] & 0xFFFF) == 0;
		c_flag_is_defined = false;
		break;
	case op_not:
		regs[i.op0] = ~regs[i.op1];
		z_flag = (regs[i.op0] & 0xFFFF) == 0;
		c_flag_is_defined = false;
		break;
	case op_shl:
		regs[i.op0] = regs[i.op1] << 1;
		z_flag = (regs[i.op0] & 0xFFFF) == 0;
		c_flag = regs[i.op0] > 0xFFFF;
		c_flag_is_defined = true;
		break;
	case op_shr:
		c_flag = regs[i.op1] & 1;
		regs[i.op0] = regs[i.op1] >> 1;
		z_flag = (regs[i.op0] & 0xFFFF) == 0;
		c_flag_is_defined = true;
		break;
#define ESC64_COND_MOV(cond, use_cary) do {\
		if((use_cary) && !c_flag_is_defined) {\
			state = UNDEFINED_EFFECT;\
			break;\
		} else {\
			if((cond)) {\
				regs[i.op0] = regs[i.op1];\
			}\
		}\
	} while(0)
	case op_mov:
		ESC64_COND_MOV(true, false);
		break;
	case op_movncnz:
		ESC64_COND_MOV(!c_flag && !z_flag, true);
		break;
	case op_movncz:
		ESC64_COND_MOV(!c_flag && z_flag, true);
		break;
	case op_movnc:
		ESC64_COND_MOV(!c_flag, true);
		break;
	case op_movcnz:
		ESC64_COND_MOV(c_flag && !z_flag, true);
		break;
	case op_movnz:
		ESC64_COND_MOV(!z_flag, false);
		break;
	case op_movnconz:
		ESC64_COND_MOV(!c_flag || !z_flag, true);
		break;
	case op_movcz:
		ESC64_COND_MOV(c_flag && z_flag, true);
		break;
	case op_movz:
		ESC64_COND_MOV(z_flag, false);
		break;
	case op_movncoz:
		ESC64_COND_MOV(!c_flag || z_flag, true);
		break;
	case op_movc:
		ESC64_COND_MOV(c_flag, true);
		break;
	case op_movconz:
		ESC64_COND_MOV(c_flag || !z_flag, true);
		break;
	case op_movcoz:
		ESC64_COND_MOV(c_flag || z_flag, true);
		break;
#define ESC64_COND_MOV_IMM(cond, use_cary) do {\
		if((use_cary) && !c_flag_is_defined) {\
			state = UNDEFINED_EFFECT;\
			break;\
		} else {\
			if(cond) {\
				regs[i.op0] = i.op3;\
			}\
		}\
	} while(0)
	case op_mov_imm:
		ESC64_COND_MOV_IMM(true, false);
		break;
	case op_movncnz_imm:
		ESC64_COND_MOV_IMM(!c_flag && !z_flag, true);
		break;
	case op_movncz_imm:
		ESC64_COND_MOV_IMM(!c_flag && z_flag, true);
		break;
	case op_movnc_imm:
		ESC64_COND_MOV_IMM(!c_flag, true);
		break;
	case op_movcnz_imm:
		ESC64_COND_MOV_IMM(c_flag && !z_flag, true);
		break;
	case op_movnz_imm:
		ESC64_COND_MOV_IMM(!z_flag, false);
		break;
	case op_movnconz_imm:
		ESC64_COND_MOV_IMM(!c_flag || !z_flag, true);
		break;
	case op_movcz_imm:
		ESC64_COND_MOV_IMM(c_flag && z_flag, true);
		break;
	case op_movz_imm:
		ESC64_COND_MOV_IMM(z_flag, false);
		break;
	case op_movncoz_imm:
		ESC64_COND_MOV_IMM(!c_flag || z_flag, true);
		break;
	case op_movc_imm:
		ESC64_COND_MOV_IMM(c_flag, true);
		break;
	case op_movconz_imm:
		ESC64_COND_MOV_IMM(c_flag || !z_flag, true);
		break;
	case op_movcoz_imm:
		ESC64_COND_MOV_IMM(c_flag || z_flag, true);
		break;
	case op_cmp: {
		uint32_t x = regs[i.op1] - regs[i.op2];
		z_flag = (x & 0xFFFF) == 0;
		c_flag = x <= 0xFFFF;
		c_flag_is_defined = true;
		}
		break;
	case op_scmp: {
		uint32_t x = regs[i.op1] - regs[i.op2];
		z_flag = (x & 0xFFFF) == 0;
		c_flag = x <= 0xFFFF;
		c_flag = ((regs[i.op1] >> 15) & 1) ^ ((regs[i.op2] >> 15) & 1) ^ (c_flag ? 1 : 0);
		c_flag_is_defined = true;
		}
		break;
	case op_ld: {
		int read_data;
		if(!safe_read_word(regs[i.op1], false, &read_data)) {
			break;
		}
		regs[i.op0] = read_data;
		}
		break;
	case op_ldb: {
		int read_data;
		if(!safe_read_byte(regs[i.op1], false, &read_data)) {
			break;
		}
		regs[i.op0] = read_data;
		}
		break;
	case op_st:
		safe_write_word(regs[i.op1], regs[i.op2], false);
		break;
	case op_stb:
		safe_write_byte(regs[i.op1], regs[i.op2] & 0xFF, false);
		break;
	case op_call:
		push(RGS_PC);
		regs[RGS_PC] = regs[i.op1];
		break;
	case op_call_imm:
		push(RGS_PC);
		regs[RGS_PC] = i.op3;
		break;
	case op_in: {
		int read_data;
		if(!safe_read_word(regs[i.op1], true, &read_data)) {
			break;
		}
		regs[i.op0] = read_data;
		}
		break;
	case op_out:
		safe_write_word(regs[i.op1], regs[i.op2], true);
		break;
	case op_push:
		push(i.op1);
		break;
	case op_pop: {
		int read_data;
		if(!safe_read_word(regs[RGS_SP], false, &read_data)) {
			break;
		}
		regs[i.op0] = read_data;
		regs[RGS_SP] += 2;
		}
		break;
	case op_halt:
		state = HALT_INSTR;
		break;
	default:
		assert(0);
		break;
	}

	for(int i = 0; i < 8; ++i) {
		regs[i] &= 0xFFFF;
	}
}

void ESC64::push(int reg) {
	assert(reg >= 0);
	assert(reg <= 8);
	regs[RGS_SP] -= 2;
	regs[RGS_SP] &= 0xFFFF;
	safe_write_word(regs[RGS_SP], regs[reg], false);
}

void ESC64::pc_next_word(void) {
	if(regs[RGS_PC] >= 0xFFFE) {
		fprintf(stderr, "ESC64: WARNING: program counter overflowed\n");
	}

	regs[RGS_PC] += 2;
	regs[RGS_PC] &= 0xFFFF;
}

bool ESC64::safe_read_word(int addr, bool select_dev, int* out_data) {
	assert(addr <= 0xFFFF);
	assert(addr >= 0);
	if(addr & 1) {
		fprintf(stderr, "ESC64: ERROR: tried to read unaligned word at %X\n", addr);
		state = IO_ERROR;
		return false;
	}
	BitVector16 bitvec = viom->read(addr >> 1, true, true, select_dev);
	bitvec.a &= 0xFFFF;
	bitvec.b &= 0xFFFF;
	if(bitvec.b != 0) {
		fprintf(stderr, "ESC64: ERROR: tried to read undefined data word at %X\n", addr);
		state = IO_ERROR;
		return false;
	}
	*out_data = bitvec.a;
	assert(*out_data <= 0xFFFF);
	assert(*out_data >= 0);
	return true;
}

bool ESC64::safe_read_byte(int addr, bool select_dev, int* out_data) {
	assert(addr <= 0xFFFF);
	assert(addr >= 0);
	bool alligned_access = (addr & 1) == 0;

	BitVector16 bitvec = viom->read(addr >> 1, !alligned_access, alligned_access, select_dev);
	if((bitvec.b & (0xFF << (alligned_access ? 0 : 8))) != 0) {
		fprintf(stderr, "ESC64: ERROR: tried to read undefined data byte at %X\n", addr);
		state = IO_ERROR;
		return false;
	}
	*out_data = (bitvec.a >> (alligned_access ? 0 : 8)) & 0xFF;

	assert(*out_data <= 0xFF);
	assert(*out_data >= 0);
	return true;
}

bool ESC64::safe_write_byte(int addr, int data, bool select_dev) {
	assert(data <= 0xFF);
	assert(data >= 0);
	assert(addr <= 0xFFFF);
	assert(addr >= 0);

	bool alligned_access = (addr & 1) == 0;
	BitVector16 bitvec;
	bitvec.b = 0;
	bitvec.a = data << (alligned_access ? 0 : 8);
	viom->write(addr >> 1, bitvec, !alligned_access, alligned_access, select_dev);

	return true;
}

bool ESC64::safe_write_word(int addr, int data, bool select_dev) {
	assert(data <= 0xFFFF);
	assert(data >= 0);
	assert(addr <= 0xFFFF);
	assert(addr >= 0);
	if(addr & 1) {
		fprintf(stderr, "ESC64: ERROR: tried to write unaligned word at %X\n", addr);
		state = IO_ERROR;
		return false;
	}

	BitVector16 bitvec;
	bitvec.b = 0;
	bitvec.a = data;
	viom->write(addr >> 1, bitvec, true, true, select_dev);

	return true;
}

void ESC64::validate_some_stuff(void) {
	//program counter is alligned
	if((regs[RGS_PC] & 1) != 0) {
		state = UNDEFINED_EFFECT;
		fprintf(stderr, "ESC64: ERROR: PC is unaligned\n");
//		assert(0);
	}

	for(int i = 0; i < 8; ++i) {
		if(regs[i] & ~0xFFFF) {
			assert(0);
		}
	}
}
