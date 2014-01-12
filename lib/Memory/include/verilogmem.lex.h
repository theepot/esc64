#ifndef VERILOGMEM_LEX_H_
#define VERILOGMEM_LEX_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	VERILOGMEM_TOKEN_ADDRESS = 1,
	VERILOGMEM_TOKEN_BINARY_WORD,
	VERILOGMEM_TOKEN_HEXADECIMAL_WORD,
	VERILOGMEM_TOKEN_COMMENT,
	VERILOGMEM_TOKEN_MULTILINE_COMMENT,
	VERILOGMEM_TOKEN_ERROR
} verilogmem_token_t;

typedef enum {
	VERILOGMEM_BIT_0 = 0,
	VERILOGMEM_BIT_1,
	VERILOGMEM_BIT_X,
	VERILOGMEM_BIT_Z,
} verilogmem_bit_t;

typedef enum {
	VERILOGMEM_FORMAT_BINARY,
	VERILOGMEM_FORMAT_HEXADECIMAL,
} verilogmem_format_t;

typedef struct {

	//Before starting the lexxer, word_width could be initialized to the required word width.
	//If initialized to -1 no word width is required and the word width will be detected. The detected width will be stored in word_width.
	int word_width;

	//Last detected value stored as verilogmem_bit_t array of size last_value_size.
	//First bit in array is lsb
	//memory for this array is allocated internally.
	verilogmem_bit_t* last_value;

	//Address of the last scanned address token.
	int address;

	//error_reason contains the reason for the error, in case of an error token.
	const char* error_reason;

	//token_text contains a copy of the last yytext
	char* token_text;

	//line_number contains the line number of the current token
	int line_number;

	//expected format. Should be initialized
	verilogmem_format_t format;

	//private:

	//Used to save the lexxer's state when going into a multiline comment
	int saved_state;



} verilogmem_process_state_t;

void start_scanner(FILE* f, verilogmem_process_state_t* ps);
void destroy_scanner(void);
extern int verilogmemlex();

#ifdef __cplusplus
}
#endif

#endif
