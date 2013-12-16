namespace cpp esc64simsrv
namespace java esc64.sim.srv

enum SimState
{
	HALTED,
	PAUSED,
	RUNNING,
	STEPPING,
	MICRO_STEPPING
}

enum ErrCode
{
	NONE,
	ILLEGAL_OPCODE,
	ILLEGAL_STATE
}

service SimService
{
	SimState getState(),
	void start(),
	void pause(),
	void microStep(),
	void step(),
	ErrCode getErrCode(),
	void reset(),
	list<i32> getRegister(1: i32 offset, 2: i32 size),
	list<i32> getMemory(1: i32 offset, 2: i32 size)
}
