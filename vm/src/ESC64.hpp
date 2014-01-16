#pragma once
#include <exception>
#include <boost/utility.hpp>
#include <cpu.h>
#include <instr_info.h>
#include <VirtualIO.hpp>
#include <stdint.h>


using namespace ::virtual_io;

class ESC64 : boost::noncopyable {
public:
	ESC64(VirtualIOManager* viom);
	~ESC64();

	bool step(void);
	void reset(void);

	//cpu state
	uint32_t regs[8];
	bool c_flag;
	bool c_flag_is_defined;
	bool z_flag;

	enum {
		break_reason_unknown_opcode = 0,
		break_reason_halt
	} break_reason;



private:
	VirtualIOManager* viom;

	struct Instr {
		opcode_t opcode;
		int op0, op1, op2, op3;
	};

	Instr fetch(void);
	bool execute(Instr i);
	//int safe_read(int addr, bool csh, bool csl, bool select_dev);
	void pc_next_word(void);
	void validate_state(void); //checks for illegal state

};
