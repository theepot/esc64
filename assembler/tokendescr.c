#include "tokendescr.h"

const TokenDescr TOKEN_DESCR_NUMBER	= { "number", TOKEN_CLASS_NUMBER, TOKEN_VALUE_TYPE_NUMBER };
const TokenDescr TOKEN_DESCR_LABEL	= { "label", TOKEN_CLASS_LABEL, TOKEN_VALUE_TYPE_STRING };
const TokenDescr TOKEN_DESCR_COMMA	= { ",", TOKEN_CLASS_PUNCTUATION, TOKEN_VALUE_TYPE_NONE };
const TokenDescr TOKEN_DESCR_COLON	= { ":", TOKEN_CLASS_PUNCTUATION, TOKEN_VALUE_TYPE_NONE };

const TokenDescr TOKEN_DESCR_OPCODE_ADD			= { "ADD", TOKEN_CLASS_OPCODE, TOKEN_VALUE_TYPE_NONE };
const TokenDescr TOKEN_DESCR_OPCODE_SUB			= { "SUB", TOKEN_CLASS_OPCODE, TOKEN_VALUE_TYPE_NONE };
const TokenDescr TOKEN_DESCR_OPCODE_OR			= { "OR", TOKEN_CLASS_OPCODE, TOKEN_VALUE_TYPE_NONE };
const TokenDescr TOKEN_DESCR_OPCODE_XOR			= { "XOR", TOKEN_CLASS_OPCODE, TOKEN_VALUE_TYPE_NONE };
const TokenDescr TOKEN_DESCR_OPCODE_AND			= { "AND", TOKEN_CLASS_OPCODE, TOKEN_VALUE_TYPE_NONE };
const TokenDescr TOKEN_DESCR_OPCODE_MOV			= { "MOV", TOKEN_CLASS_OPCODE, TOKEN_VALUE_TYPE_NONE };
const TokenDescr TOKEN_DESCR_OPCODE_MOV_EQ		= { "MOV_EQ", TOKEN_CLASS_OPCODE, TOKEN_VALUE_TYPE_NONE };
const TokenDescr TOKEN_DESCR_OPCODE_MOV_NEQ		= { "MOV_NEQ", TOKEN_CLASS_OPCODE, TOKEN_VALUE_TYPE_NONE };
const TokenDescr TOKEN_DESCR_OPCODE_MOV_LESS		= { "MOV_LESS", TOKEN_CLASS_OPCODE, TOKEN_VALUE_TYPE_NONE };
const TokenDescr TOKEN_DESCR_OPCODE_MOV_LESS_EQ	= { "MOV_LESS_EQ", TOKEN_CLASS_OPCODE, TOKEN_VALUE_TYPE_NONE };
const TokenDescr TOKEN_DESCR_OPCODE_CMP			= { "CMP", TOKEN_CLASS_OPCODE, TOKEN_VALUE_TYPE_NONE };
const TokenDescr TOKEN_DESCR_PSEUDO_OPCODE_LDR	= { "?LDR?", TOKEN_CLASS_PSEUDO_OPCODE, TOKEN_VALUE_TYPE_NONE };
const TokenDescr TOKEN_DESCR_OPCODE_LDR			= { "LDR", TOKEN_CLASS_OPCODE, TOKEN_VALUE_TYPE_NONE };
const TokenDescr TOKEN_DESCR_OPCODE_LDR_LIT		= { "LDR_LIT", TOKEN_CLASS_OPCODE, TOKEN_VALUE_TYPE_NONE };
const TokenDescr TOKEN_DESCR_PSEUDO_OPCODE_STR	= { "?STR?", TOKEN_CLASS_PSEUDO_OPCODE, TOKEN_VALUE_TYPE_NONE };
const TokenDescr TOKEN_DESCR_OPCODE_STR			= { "STR", TOKEN_CLASS_OPCODE, TOKEN_VALUE_TYPE_NONE };
const TokenDescr TOKEN_DESCR_OPCODE_STR_LIT		= { "STR_LIT", TOKEN_CLASS_OPCODE, TOKEN_VALUE_TYPE_NONE };
const TokenDescr TOKEN_DESCR_OPCODE_CALL			= { "CALL", TOKEN_CLASS_OPCODE, TOKEN_VALUE_TYPE_NONE };

const TokenDescr TOKEN_DESCR_DIR_WORD	= { "WORD", TOKEN_CLASS_DIRECTIVE, TOKEN_VALUE_TYPE_NONE };

const TokenDescr TOKEN_DESCR_EOL		= { "EOL", TOKEN_CLASS_PUNCTUATION, TOKEN_VALUE_TYPE_NONE };
