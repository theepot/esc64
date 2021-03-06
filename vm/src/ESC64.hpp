#pragma once
#include <exception>
#include <boost/utility.hpp>
#include <boost/thread.hpp>
#include <cpu.h>
#include <instr_info.h>
#include <VirtualIO.hpp>
#include <stdint.h>
#include <unistd.h>

using namespace ::virtual_io;

class ESC64 : boost::noncopyable {
private:
	enum {
		TICKS_PER_INST	= 4,
		TIME_PER_TICK	= 5		//5 microseconds = 1 / 200 000 000 (200kHz)
	};

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
		IO_ERROR,
		UNDEFINED_EFFECT,
	} state;

	int64_t get_step_count() { return step_count; }



private:
	int64_t step_count;
	int64_t time_started;

	VirtualIOManager* viom;

	struct Instr {
		opcode_t opcode;
		int op0, op1, op2, op3;
	};

	bool fetch(ESC64::Instr* out_instr);
	int64_t time_now(); //returns the time in microseconds
	void delay();
	void execute(Instr i);
	bool safe_read_word(int addr, bool select_dev, int* out_data);
	bool safe_read_byte(int addr, bool select_dev, int* out_data);
	bool safe_write_word(int addr, int data, bool select_dev);
	bool safe_write_byte(int addr, int data, bool select_dev);
	void pc_next_word(void);
	void validate_some_stuff(void); //checks for illegal state

	void push(int reg);

};
