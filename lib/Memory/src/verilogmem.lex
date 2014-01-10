%{

/*
	Strongly modified version of Stephen Williams's sys_readmem_lex.lex from the Icarus Verilog project.
	See http://iverilog.icarus.com
*/

#include <verilogmem.lex.h>
#include <stdio.h>
#include <assert.h>

static const char* verilogmem_errormsg_invalid_notation = "invalid notation";
static const char* verilogmem_errormsg_invalid_address = "invalid address";
static const char* verilogmem_errormsg_word_width_exceeded = "to much data in word";
static const char* verilogmem_errormsg_word_width_to_small = "not enough data in word";
static const char* verilogmem_errormsg_expected_hex = "expected hexadecimal notation";
static const char* verilogmem_errormsg_expected_bin = "expected binary notation";

static verilogmem_process_state_t* process_state;

static int process_multi_line_comment(void);
static void process_multi_line_comment_begin(void);
static int process_single_line_comment(void);
static int process_address(void);
static int process_hex_value(void);
static int process_bin_value(void);
static int invalid_notation_error(void);

%}
%option prefix="verilogmem"
%option nounput
%option noinput
%option noyywrap
%option yylineno


%x BIN
%x HEX
%x COMMENT

%%

<HEX,BIN>"//".* { return process_single_line_comment(); }
<HEX,BIN>[ \t\f\n\r] { ; }

<HEX,BIN>@[0-9a-fA-F]+ { return process_address(); }
<HEX>[0-9a-fA-FxXzZ_]+  { return process_hex_value(); }
<BIN>[01xXzZ_]+  { return process_bin_value(); }

<HEX,BIN>"/*"   { process_multi_line_comment_begin(); }
<COMMENT>[^*]* { yymore(); }
<COMMENT>"*"   { yymore(); }
<COMMENT>"*"+"/" { BEGIN(process_state->saved_state); return process_multi_line_comment(); }

<HEX,BIN>. { return invalid_notation_error(); }

%%


#ifdef __CDT_PARSER__
	#define verilogmemlineno x
	#define verilogmemtext x
	#define verilogmemleng x
	#define YY_START x
	#define HEX x
	#define BIN x
	#define COMMENT x
	static verilogmem_process_state_t* process_state;
#endif

static int process_multi_line_comment(void) {
	process_state->line_number = verilogmemlineno;
	process_state->token_text = verilogmemtext;
	assert(verilogmemleng >= 2);
	process_state->token_text[verilogmemleng-2] = '\0';
	return VERILOGMEM_TOKEN_MULTILINE_COMMENT;
}

static void process_multi_line_comment_begin(void) {
	process_state->line_number = verilogmemlineno;
	process_state->saved_state = YY_START; BEGIN(COMMENT);
	process_state->token_text = verilogmemtext + 2;
}

static int process_single_line_comment(void) {
	process_state->line_number = verilogmemlineno;
	process_state->token_text = verilogmemtext + 2;
	return VERILOGMEM_TOKEN_COMMENT;
}

static int process_address(void) {
	process_state->line_number = verilogmemlineno;
	if(sscanf(verilogmemtext+1, "%x", &process_state->address) < 1) {
		process_state->token_text = verilogmemtext;
		process_state->error_reason = verilogmem_errormsg_invalid_address;
		return VERILOGMEM_TOKEN_ERROR;		
	} else {
		return VERILOGMEM_TOKEN_ADDRESS;
	}
}

static int minimal_needed_hex_digits(const int word_width) {
	return word_width / 4 + ((word_width % 4) == 0 ? 0 : 1);
}

static void clear_last_value(verilogmem_process_state_t* ps) {
	int n;
	for(n = 0; n < ps->word_width; ++n) {
		ps->last_value[n] = VERILOGMEM_BIT_0;
	}
}

//returns -1 for z and Z. Returns -2 for x and X. Returns < -2 in case of a invalid character
static int hex_value(char c) {
	if(c >= '0' && c <= '9') {
		return c - '0';
	} else if(c >= 'a' && c <= 'f') {
		return c - 'a' + 10;
	} else if(c >= 'A' && c <= 'F') {
		return c - 'A' + 10;
	} else if(c == 'z' || c == 'Z') {
		return -1;
	} else if(c == 'x' || c == 'X') {
		return -2;
	} else {
		return -3;
	}
}

//returns -1 for z and Z. Returns -2 for x and X. Returns < -2 in case of a invalid character
static int bin_value(char c) {
	if(c == '0') {
		return 0;
	} else if(c == '1') {
		return 1;
	} else if(c == 'z' || c == 'Z') {
		return -1;
	} else if(c == 'x' || c == 'X') {
		return -2;
	} else {
		return -3;
	}
}

static int check_word_width(const char* text, int char_width, int (*func_charvalue)(char)) {
	const char* c = text;
	int word_width = 0;
	while(*c != '\0') {
		int val = (*func_charvalue)(*c);
		if(val >= -2)
			word_width += char_width;
		c++;
	}

	return word_width;
}

static void allocate_last_value(verilogmem_process_state_t* ps) {
	assert(process_state->word_width > 0);
	process_state->last_value = calloc(process_state->word_width, sizeof(verilogmem_bit_t));
	assert(process_state->last_value != NULL);
}

static void detect_word_width_and_allocate(verilogmem_process_state_t* ps) {
	assert(ps->word_width == -1);

	switch(ps->format) {
		case VERILOGMEM_FORMAT_HEXADECIMAL:
			process_state->word_width = check_word_width(verilogmemtext, 4, &hex_value);
			break;
		case VERILOGMEM_FORMAT_BINARY:
			process_state->word_width = check_word_width(verilogmemtext, 1, &bin_value);
			break;
		default:
			assert(0);
			break;
	}

	allocate_last_value(ps);
}

static const char* error_reason_expected_word_format(const verilogmem_process_state_t* ps) {
	switch(ps->format) {
	case VERILOGMEM_FORMAT_BINARY:
		return verilogmem_errormsg_expected_bin;
	case VERILOGMEM_FORMAT_HEXADECIMAL:
		return verilogmem_errormsg_expected_hex;
	default:
		assert(0);
		break;
	}
}

static int process_hex_value(void) {
	process_state->line_number = verilogmemlineno;
	process_state->token_text = verilogmemtext;

	if(process_state->format != VERILOGMEM_FORMAT_HEXADECIMAL) {
		process_state->error_reason = error_reason_expected_word_format(process_state);
		return VERILOGMEM_TOKEN_ERROR;
	}

	if(process_state->word_width == -1) {
		detect_word_width_and_allocate(process_state);
	}

	clear_last_value(process_state);

	int nibble = 0;
	char* c;
	for(c = verilogmemtext + verilogmemleng - 1; c >= verilogmemtext; --c) {
		if(*c == '_')
			continue;

		if(nibble + 1 > minimal_needed_hex_digits(process_state->word_width)) {
			process_state->error_reason = verilogmem_errormsg_word_width_exceeded;
			return VERILOGMEM_TOKEN_ERROR;
		}

		int nibble_val = hex_value(*c);
		
		assert(nibble_val >= -2 && nibble_val <= 15);

		if(nibble_val >= 0) {
			int n;
			for(n = 0; n < 4; ++n) {
				if((nibble_val >> n) & 1) {
					if(nibble*4 + n >= process_state->word_width) {
						process_state->error_reason = verilogmem_errormsg_word_width_exceeded;
						return VERILOGMEM_TOKEN_ERROR;
					}
					process_state->last_value[nibble*4 + n] = VERILOGMEM_BIT_1;
				}
			}
		} else {
			verilogmem_bit_t val = nibble_val == -1 ? VERILOGMEM_BIT_Z : VERILOGMEM_BIT_X;
			int n;
			int n_max = 4;
			if(nibble*4 + 3 >= process_state->word_width) {
				n_max = process_state->word_width % 4;
			}
			for(n = 0; n < n_max; ++n) {
				process_state->last_value[nibble*4 + n] = val;
			}
		}
		
		nibble++;
	}

	if(nibble < minimal_needed_hex_digits(process_state->word_width)) {
		process_state->error_reason = verilogmem_errormsg_word_width_to_small;
		return VERILOGMEM_TOKEN_ERROR;
	}

	return VERILOGMEM_TOKEN_HEXADECIMAL_WORD;
}

static int process_bin_value(void) {
	process_state->line_number = verilogmemlineno;
	process_state->token_text = verilogmemtext;

	if(process_state->format != VERILOGMEM_FORMAT_BINARY) {
		process_state->error_reason = error_reason_expected_word_format(process_state);
		return VERILOGMEM_TOKEN_ERROR;
	}

	if(process_state->word_width == -1) {
		detect_word_width_and_allocate(process_state);
	}

	clear_last_value(process_state);

	int bit = 0;
	char* c;
	for(c = verilogmemtext + verilogmemleng - 1; c >= verilogmemtext; --c) {
		if(*c == '_')
			continue;

		if(bit + 1 > process_state->word_width) {
			process_state->error_reason = verilogmem_errormsg_word_width_exceeded;
			return VERILOGMEM_TOKEN_ERROR;
		}

		int bit_val = bin_value(*c);

		assert(bit_val >= -2 && bit_val <= 1);

		verilogmem_bit_t val;
		switch(bit_val) {
		case 0:
			val = VERILOGMEM_BIT_0;
			break;
		case 1:
			val = VERILOGMEM_BIT_1;
			break;
		case -1:
			val = VERILOGMEM_BIT_Z;
			break;
		case -2:
			val = VERILOGMEM_BIT_X;
			break;
		default:
			assert(0);
		}

		process_state->last_value[bit] = val;

		bit++;

	}

	return VERILOGMEM_TOKEN_BINARY_WORD;
}

static int invalid_notation_error(void) {
	process_state->line_number = verilogmemlineno;
	process_state->token_text = verilogmemtext;
	process_state->error_reason = verilogmem_errormsg_invalid_notation;
	return VERILOGMEM_TOKEN_ERROR;
}

void start_scanner(FILE* f, verilogmem_process_state_t* ps) {
	verilogmemrestart(f);
	process_state = ps;

	if(process_state->word_width != -1) {
		allocate_last_value(process_state);
	}

	if(ps->format == VERILOGMEM_FORMAT_BINARY) {
		BEGIN(BIN);
	} else if(ps->format == VERILOGMEM_FORMAT_HEXADECIMAL) {
		BEGIN(HEX);
	}
}

void destroy_scanner(void) {
	verilogmemlex_destroy();
	free(process_state->last_value);
}
