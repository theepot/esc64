#include <cstdio>
#include <cassert>

#include <verilogmem.lex.h>

void print_value(const verilogmem_bit_t* bits, int size) {
	putchar('[');
	for(int n = 0; n < size; ++n) {
		switch(bits[n]) {
		case VERILOGMEM_BIT_0:
			putchar('0');
			break;
		case VERILOGMEM_BIT_1:
			putchar('1');
			break;
		case VERILOGMEM_BIT_X:
			putchar('x');
			break;
		case VERILOGMEM_BIT_Z:
			putchar('z');
			break;
		default:
			printf("(INVALID BIT %d)", bits[n]);
			break;
		}
	}

	putchar(']');
}


int main() {
	FILE* f = fopen("test.lst", "r");

	verilogmem_process_state_t ps;
	ps.word_width = -1;
	ps.format = VERILOGMEM_FORMAT_BINARY;
	start_scanner(f, &ps);

	int token;
	while((token = verilogmemlex()) != 0) {
		switch(token) {
		case VERILOGMEM_TOKEN_BINARY_WORD:
			printf("[%d] bin token: ", ps.line_number);
			print_value(ps.last_value, ps.word_width);
			putchar('\n');
			break;
		case VERILOGMEM_TOKEN_HEXADECIMAL_WORD:
			printf("[%d] hex token: ", ps.line_number);
			print_value(ps.last_value, ps.word_width);
			putchar('\n');
			break;
		case VERILOGMEM_TOKEN_ERROR:
			printf("[%d] error token: \"%s\". reason: %s\n", ps.line_number, ps.token_text, ps.error_reason);
			break;
		case VERILOGMEM_TOKEN_ADDRESS:
			printf("[%d] address token: 0x%X\n", ps.line_number, ps.address);
			break;
		case VERILOGMEM_TOKEN_COMMENT:
			printf("[%d] comment token: %s\n", ps.line_number, ps.token_text);
			break;
		case VERILOGMEM_TOKEN_MULTILINE_COMMENT:
			printf("[%d] multiline comment token: %s\n", ps.line_number, ps.token_text);
			break;
		default:
			printf("[%d] unimplemented token. token number %d\n", ps.line_number, token);
			break;
		}
	}

	printf("word width was %d\n", ps.word_width);

	fclose(f);

	return 0;
}
