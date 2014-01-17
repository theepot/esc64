namespace cpp esc64controlsrv
namespace java esc64.control.srv

enum ComputerState
{
	RUNNING,
	PAUSED,
	UNKNOWN_OPCODE,
	HALT_INSTR,
	HW_FAULT,
	READ_ERROR,
	UNDEFINED_EFFECT,
	BREAKPOINT,
}

enum CarryState
{
	SET,
	UNSET,
	UNDEFINED
}

service ComputerControlService
{
	ComputerState getState(),
	void start(),
	void pause(),
	void microStep(),
	void step(),
	void reset(),
	CarryState getCarryFlag(),
	bool getZeroFlag(),
	i64 getInstrCount(),
	i64 getClockCount(),
	list<i32> getRegister(1: i32 offset, 2: i32 size),
	list<i32> getMemory(1: i32 offset, 2: i32 size)
}
