#include <stdio.h>
#include <vpi_user.h>

static int TickCompile(char*user_data);
static int TickCall(char*user_data);
void TickRegister();

void (*vlog_startup_routines[])() = { TickRegister };

void TickRegister()
{
      s_vpi_systf_data tfTick;

      tfTick.type      = vpiSysTask;
      tfTick.tfname    = "$tick";
      tfTick.calltf    = TickCall;
      tfTick.compiletf = TickCompile;
      tfTick.sizetf    = 0;
      tfTick.user_data = 0;
      vpi_register_systf(&tfTick);
}

static int TickCompile(char* unused)
{
	return 0;
}

static int TickCall(char* unused)
{
	puts("=== TickCall() ===");
	return 0;
}

