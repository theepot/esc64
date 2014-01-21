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

	void step(void);
	void reset(void);

	//cpu state
	uint32_t regs[8];
	bool c_flag;
	bool c_flag_is_defined;
	bool z_flag;


	enum {
		OK,
		UNKNOWN_OPCODE,
		HALT_INSTR,
		READ_ERROR,
		UNDEFINED_EFFECT,
	} state;

	int64_t get_step_count() { return step_count; }



private:
	int64_t step_count;

	VirtualIOManager* viom;

	struct Instr {
		opcode_t opcode;
		int op0, op1, op2, op3;
	};

	bool fetch(ESC64::Instr* out_instr);
	void execute(Instr i);
	bool safe_read_word(int addr, bool csh, bool csl, bool select_dev, int* out_data);
	void pc_next_word(void);
	void validate_some_stuff(void); //checks for illegal state

};
