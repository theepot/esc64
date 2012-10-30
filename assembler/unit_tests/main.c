extern void TestScanner(const char* asmFile);
extern int TestSymTable(void);

int main(int argc, char** argv)
{
	TestScanner("/home/lukas/development/esc64/assembler/testfiles/scanner_test.asm");
	TestSymTable();

	return 0;
}
