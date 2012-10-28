
extern void TestScanner(const char* asmFile);

int main(int argc, char** argv)
{
	TestScanner("/home/lukas/development/esc64/assembler/testfiles/scanner_test.asm");
	return 0;
}
