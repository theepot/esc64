namespace cpp esc64simsrv
namespace java esc64.sim.srv

enum SimState
{
	STOPPED,
	PAUSED,
	RUNNING,
	STEPPING,
}

service SimService
{
	SimState getState(),
	void start(),
	void stop(),
	void pause(),
	void step(),
	list<i16> getRegister(1: i32 offset, 2: i32 size),
	list<i16> getMemory(1: i32 offset, 2: i32 size)
}
