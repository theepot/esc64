#include "scanner.h"
#include "symtable.h"

#include <assert.h>
#include <stdio.h>

static int TestScanner(int argc, char** argv);
static int TestSymTable(int argc, char** argv);

int main(int argc, char** argv)
{
	puts("BEGIN");

	int e = TestScanner(argc, argv);
	//int e = TestSymTable(argc, argv);
	
	puts("");
	puts("END");
	
	return e;
}

static int TestScanner(int argc, char** argv)
{
	assert(argc == 2);
	FILE* scannerInput = fopen(argv[1], "r");
	assert(scannerInput);
	
	Scanner scanner;
	ScannerInit(&scanner, scannerInput);

	const int maxScans = 200;
	int scans = 0;
	unsigned int curLine = 1;

	printf("line 001:");
	Token token;
	while(ScannerNext(&scanner, &token))
	{
		putchar(' ');
		ScannerDumpToken(stdout, &token);
		
		if(curLine != scanner.line)
		{
			curLine = scanner.line;
			printf("\nline %03d:", curLine);
		}
		
		if(++scans > maxScans)
		{
			fprintf(stderr, "max scans reached\n");
			break;
		}
	}

	return 0;
}

static int TestSymTable(int argc, char** argv)
{
	SymTable symTable;
	SymTableInit(&symTable);
	
	SymTableInsert(&symTable, "abc", 123);
	SymTableInsert(&symTable, "xyz", 456);
	
	const SymBucket* abc = SymTableFind(&symTable, "abc");
	const SymBucket* pqr = SymTableFind(&symTable, "pqr");
	
	printf("abc %s, pqr %s\n", abc ? "found" : "not found", pqr ? "found" : "not found");
	
	SymTableDump(stdout, &symTable);
}
















