#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include <esc64asm/reswords.h>

extern void TestScanner(const char* asmFile);
extern int TestSymTable(void);
extern void TestObjFile(const char* path);
extern void TestReservedWords(void);
extern void TestParser(const char* asmFile, const char* objFile);
extern void TestRecords(const char* filePath);
extern void TestObjCode(const char* fileName);
extern void TestExecWriteRead(const char* filePath);

#ifndef ESC_TEST_FILE_BASE_PATH
#error no base path for test files defined
#endif

int main(int argc, char** argv)
{
	clock_t start = clock();

	ReservedWordsInit();

	TestScanner(ESC_TEST_FILE_BASE_PATH"/scanner_test.asm");
	TestSymTable();
	TestReservedWords();
	TestParser(ESC_TEST_FILE_BASE_PATH"/scanner_test.asm", ESC_TEST_FILE_BASE_PATH"/parser_test.o");
	TestRecords(ESC_TEST_FILE_BASE_PATH"/records.bin");
	TestObjCode(ESC_TEST_FILE_BASE_PATH"/test.o");
	TestExecWriteRead(ESC_TEST_FILE_BASE_PATH"/assembler_exec_test");

	clock_t stop = clock();
	clock_t ms = (stop - start) / 1000;
	printf("Tests completed successfully in %lums\n", ms);

	return 0;
}
