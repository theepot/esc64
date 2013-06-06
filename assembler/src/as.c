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

	ScannerInit(asmPath);

	ObjectWriter objWriter;
	ObjectWriterInit(&objWriter, objPath);

	ParserInit(&objWriter);
	Parse();

	ObjectWriterClose(&objWriter);
	ScannerClose();

	return 0;
}
