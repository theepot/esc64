#include <stdlib.h>
#include <assert.h>
#include <esc64asm/objcode.h>
#include <esc64asm/link.h>

int main(int argc, char** argv)
{
	puts("wtf?");

	assert(argc >= 3);
	const char* exeFile = argv[1];
	const char** objFiles = (const char**)&argv[2];
	size_t objFileCount = argc - 2;

	Link(exeFile, objFiles, objFileCount);

	return 0;
}