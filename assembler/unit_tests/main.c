#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "reswords.h"

extern void TestScanner(const char* asmFile);
extern int TestSymTable(void);
extern void TestObjFile(const char* path);
extern void TestReservedWords(void);
extern void TestParser(const char* testFile);
extern void TestRecords(const char* filePath);
extern void TestObjCode(const char* fileName);

int main(int argc, char** argv)
{
	clock_t start = clock();

	ReservedWordsInit();

	//TestScanner("/home/lukas/slowpoke/esc64/assembler/testfiles/scanner_test.asm");
//	TestSymTable();
//	TestObjFile("/home/lukas/Desktop/dump.bin");
//	TestReservedWords();
//	TestParser("/home/lukas/development/esc64/assembler/testfiles/scanner_test.asm");
//	TestRecords("/home/lukas/Desktop/records.bin");
	TestObjCode("/home/lukas/Desktop/test.o");

	clock_t stop = clock();
	clock_t ms = (stop - start) / 1000;
	printf("Tests completed successfully in %lums\n", ms);

	return 0;
}
