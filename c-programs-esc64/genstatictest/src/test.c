#include <esctest.h>
#include <esccrt.h>

static const char* static_global_string = "\x01\x02\x03";

int main(int argc, char** argv)
{
	static const char* static_local_string = "\x11\x12\x13";
	const char* local_string = "\x21\x22\x23";

	(void)static_local_string;
	(void)local_string;
	(void)static_global_string;

	return 0;	
}
