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
		step_count++;
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
	if(!safe_read_word(regs[RGS_PC], true, true, false, &read_data)) {
		return false;
	}

	Instr result;
	result.opcode = (opcode_t)((read_data >> 9) & 0x01FF);
	if(instr_info[result.opcode].opcode == 0) {
		state = UNKNOWN_OPCODE;
		return false;
	}
	result.op0 = (read_data >> 6) & 0x7;
	result.op1 = (read_data >> 3) & 0x7;
	result.op2 = (read_data >> 0) & 0x7;

	pc_next_word();

	if(instr_info[result.opcode].wide) {
		if(!safe_read_word(regs[RGS_PC], true, true, false, &read_data)) {
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
		regs[i.op0] = regs[i.op1] + regs[i.op2] + c_flag ? 1 : 0;
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
		regs[i.op0] = regs[i.op1] - regs[i.op2] - 1 + c_flag ? 1 : 0;
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
	case op_ldr: {
		int read_data;
		if(!safe_read_word(regs[i.op1], true, true, false, &read_data)) {
			break;
		}
		regs[i.op0] = read_data;
		}
		break;
	case op_str: {
		BitVector16 bitvec;
		bitvec.b = 0;
		bitvec.a = regs[i.op2];
		viom->write(regs[i.op1] >> 1, bitvec, true, true, false);
		}
		break;
	case op_call:
		regs[RGS_LR] = regs[RGS_PC];
		regs[RGS_PC] = regs[i.op1];
		break;
	case op_call_imm:
		regs[RGS_LR] = regs[RGS_PC];
		regs[RGS_PC] = i.op3;
		break;
	case op_in: {
		int read_data;
		if(!safe_read_word(regs[i.op1], true, true, true, &read_data)) {
			break;
		}
		regs[i.op0] = read_data;
		}
		break;
	case op_out: {
		BitVector16 bitvec;
		bitvec.b = 0;
		bitvec.a = regs[i.op2];
		viom->write(regs[i.op1] >> 1, bitvec, true, true, true);
		}
		break;
	case op_push: {
		regs[RGS_SP] -= 2;
		BitVector16 bitvec;
		bitvec.b = 0;
		bitvec.a = regs[i.op1];
		viom->write(regs[RGS_SP] >> 1, bitvec, true, true, false);
		}
		break;
	case op_pop: {
		int read_data;
		if(!safe_read_word(regs[RGS_SP], true, true, false, &read_data)) {
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

void ESC64::pc_next_word(void) {
	if(regs[RGS_PC] >= 0xFFFE) {
		fprintf(stderr, "ESC64: WARNING: program counter overflowed\n");
	}

	regs[RGS_PC] += 2;
}

bool ESC64::safe_read_word(int addr, bool csh, bool csl, bool select_dev, int* out_data) {
	if(addr & 1) {
		fprintf(stderr, "ESC64: ERROR: tried to read unaligned word\n");
		state = READ_ERROR;
		return false;
	}
	BitVector16 bitvec = viom->read(addr >> 1, csh, csh, select_dev);
	if(bitvec.b != 0) {
		state = READ_ERROR;
		return false;
	}
	*out_data = bitvec.a;
	return true;
}

void ESC64::validate_some_stuff(void) {
	if((regs[RGS_PC] & 1) != 0) {
		assert(0);
	}

	for(int i = 0; i < 8; ++i) {
		if(regs[i] & ~0xFFFF) {
			assert(0);
		}
	}
}