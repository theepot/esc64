#include <assert.h>
#include <esc64asm/scanner.h>
#include <esc64asm/objwrite.h>
#include <esc64asm/parser.h>

#define USAGE \
	"usage: esc-as OUTPUT INPUT\n" \
	"\tassembles assembly file INPUT into object OUTPUT\n"

int main(int argc, char** argv)
{
	if(argc != 3)
	{
		fputs(USAGE, stderr);
		return 1;
	}

	const char* objPath = argv[1];
	const char* asmPath = argv[2];
	Parse(asmPath, objPath);

	return 0;
}
