namespace cpp esc64sim
namespace java esc64.sim

enum State
{
	STOPPED,
	PAUSED,
	RUNNING,
	STEPPING,
}

service Service
{
	State getState(),
	void start(),
	void stop(),
	void pause(),
	void step(),
	list<i16> getRegister(1: i32 offset, 2: i32 size),
	list<i16> getMemory(1: i32 offset, 2: i32 size)
}
