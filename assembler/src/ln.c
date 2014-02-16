#include <stdlib.h>
#include <assert.h>
#include <esc64asm/objread.h>
#include <esc64asm/link.h>

#define USAGE \
	"usage: esc-ln OUTPUT INPUT...\n" \
	"\tlinks multiple object files (INPUT) into an executable (OUTPUT)\n"

int main(int argc, char** argv)
{
	if(argc < 3)
	{
		fputs(USAGE, stderr);
		return 1;
	}

	const char* exeFile = argv[1];
	const char** objFiles = (const char**)&argv[2];
	size_t objFileCount = argc - 2;

	ExeWriter exeWriter;
	ExeWriterInit(&exeWriter, exeFile);
	Link(&exeWriter, objFiles, objFileCount);

	return 0;
}
