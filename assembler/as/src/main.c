#include <assert.h>
#include <scanner.h>
#include <objcode.h>
#include <parser.h>
#include <reswords.h>

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
