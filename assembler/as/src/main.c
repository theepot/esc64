#include <assert.h>
#include <esc64asm/scanner.h>
#include <esc64asm/objcode.h>
#include <esc64asm/parser.h>
#include <esc64asm/reswords.h>

int main(int argc, char** argv)
{
	assert(argc == 3);
	const char* objPath = argv[1];
	const char* asmPath = argv[2];

	ReservedWordsInit();

	Scanner scanner;
	ScannerInit(&scanner, asmPath);

	ObjectWriter objWriter;
	ObjectWriterInit(&objWriter, objPath);

	Parser parser;
	ParserInit(&parser, &scanner, &objWriter);

	Parse(&parser);

	ObjectWriterClose(&objWriter);
	ScannerClose(&scanner);

	return 0;
}
