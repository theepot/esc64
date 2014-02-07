#include <esccrt.h>
#include <escio.h>

char bytes[] = "123456789";
#define BYTES_N		(sizeof bytes / sizeof bytes[0])

int words[] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G' };
#define WORDS_N		(sizeof words / sizeof words[0])

int main(int argc, char** argv)
{
	io_outs(0xAAAA, "hello world\n");
	io_outac(0xAAAA, bytes, BYTES_N);
	io_out(0xAAAA, '\n');
	io_outai(0xAAAA, words, WORDS_N);
	io_out(0xAAAA, '\n');
	
	return 0;
}
