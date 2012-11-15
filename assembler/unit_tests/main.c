#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

extern void TestScanner(const char* asmFile);
extern int TestSymTable(void);
extern void TestObjFile(const char* path);

int main(int argc, char** argv)
{
	clock_t start = clock();
	TestScanner("/home/lukas/development/esc64/assembler/testfiles/scanner_test.asm");
	TestSymTable();
	TestObjFile("/home/lukas/Desktop/dump.bin");
	clock_t stop = clock();
	clock_t ms = (stop - start) / 1000;
	printf("Tests completed successfully in %ums\n", ms);

	return 0;
}
