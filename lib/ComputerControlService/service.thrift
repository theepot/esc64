namespace cpp esc64controlsrv
namespace java esc64.control.srv

enum ComputerState
{
	RUNNING,
	PAUSED,
	UNKNOWN_OPCODE,
	HALT_INSTR,
	HW_FAULT,
	IO_ERROR,
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
	void quit(), //quits the vm or simulation
	CarryState getCarryFlag(),
	bool getZeroFlag(),
	i64 getInstrCount(),
	i64 getClockCount(),
	list<i32> getRegister(1: i32 offset, 2: i32 size),
	
	//offset and size are specified in bytes. Returns array of i32. Every i32 encodes one byte.
	//Each bit in a byte is encoded with two successive bits in the i32. The format is: 0b0000000000000000abcdefghabcdefgh
	//msb-a lsb-a  val
	//0     0      0
	//0     1      z
	//1     0      1
	//1     1      x
	//the a's encode the most significant bit. The h's the least significant bit.
	list<i32> getMemory(1: i32 offset, 2: i32 size)
}
