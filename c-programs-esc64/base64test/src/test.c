#include <stdesc.h>
#include <base64.h>

const char str[] = "Hello, I will be encoded and then decoded using BASE64.";

int main(int argc, char** argv)
{
	size_t n;
	char encoded[256];
	char decoded[256];
	
	base64enc(encoded, str, sizeof str);
	n = base64_binlength(encoded);
	base64dec(decoded, encoded);
	
	puts(str);
	puts(encoded);
	puts(decoded);
	
	return 0;
}
