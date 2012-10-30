#include "tokendescr.h"

#include <stdlib.h>

const TokenDescr TOKEN_DESCR_LABEL_DECL			= { "ldecl", TOKEN_CLASS_LABEL_DECL, TOKEN_VALUE_TYPE_STRING, NULL };
const TokenDescr TOKEN_DESCR_NUMBER				= { "number", TOKEN_CLASS_LVALUE, TOKEN_VALUE_TYPE_NUMBER, NULL };
const TokenDescr TOKEN_DESCR_LABEL_REF			= { "lref", TOKEN_CLASS_LVALUE, TOKEN_VALUE_TYPE_STRING, NULL };
const TokenDescr TOKEN_DESCR_REGISTER_REF		= { "reg", TOKEN_CLASS_LVALUE, TOKEN_VALUE_TYPE_NUMBER, NULL };

//opcodes
const TokenDescr TOKEN_DESCR_OPCODE_ADD			= { "ADD", TOKEN_CLASS_OPCODE, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_ADD };
const TokenDescr TOKEN_DESCR_OPCODE_SUB			= { "SUB", TOKEN_CLASS_OPCODE, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_SUB };
const TokenDescr TOKEN_DESCR_OPCODE_OR			= { "OR", TOKEN_CLASS_OPCODE, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_OR };
const TokenDescr TOKEN_DESCR_OPCODE_XOR			= { "XOR", TOKEN_CLASS_OPCODE, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_XOR };
const TokenDescr TOKEN_DESCR_OPCODE_AND			= { "AND", TOKEN_CLASS_OPCODE, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_AND };
const TokenDescr TOKEN_DESCR_PSEUDO_OPCODE_MOV	= { "MOV?", TOKEN_CLASS_PSEUDO_OPCODE, TOKEN_VALUE_TYPE_NONE, NULL };
const TokenDescr TOKEN_DESCR_OPCODE_MOV			= { "MOV", TOKEN_CLASS_OPCODE, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_MOV };
const TokenDescr TOKEN_DESCR_OPCODE_MOV_WIDE	= { "MOV", TOKEN_CLASS_OPCODE, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_MOV_WIDE };
const TokenDescr TOKEN_DESCR_OPCODE_MOV_EQ		= { "MOV_EQ", TOKEN_CLASS_OPCODE, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_MOV_EQ };
const TokenDescr TOKEN_DESCR_OPCODE_MOV_NEQ		= { "MOV_NEQ", TOKEN_CLASS_OPCODE, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_MOV_NEQ };
const TokenDescr TOKEN_DESCR_OPCODE_MOV_LESS	= { "MOV_LESS", TOKEN_CLASS_OPCODE, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_MOV_LESS };
const TokenDescr TOKEN_DESCR_OPCODE_MOV_LESS_EQ	= { "MOV_LESS_EQ", TOKEN_CLASS_OPCODE, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_MOV_LESS_EQ };
const TokenDescr TOKEN_DESCR_OPCODE_CMP			= { "CMP", TOKEN_CLASS_OPCODE, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_CMP };
const TokenDescr TOKEN_DESCR_OPCODE_LDR			= { "LDR", TOKEN_CLASS_OPCODE, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_LDR };
const TokenDescr TOKEN_DESCR_OPCODE_STR			= { "STR", TOKEN_CLASS_OPCODE, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_STR };
const TokenDescr TOKEN_DESCR_OPCODE_CALL		= { "CALL", TOKEN_CLASS_OPCODE, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_CALL };

//directives
const TokenDescr TOKEN_DESCR_DIR_WORD			= { "WORD", TOKEN_CLASS_DIRECTIVE, TOKEN_VALUE_TYPE_NONE, NULL };
const TokenDescr TOKEN_DESCR_DIR_ALIGN			= { "ALIGN", TOKEN_CLASS_DIRECTIVE, TOKEN_VALUE_TYPE_NONE, NULL };
const TokenDescr TOKEN_DESCR_DIR_ASCII			= { "ASCII", TOKEN_CLASS_DIRECTIVE, TOKEN_VALUE_TYPE_NONE, NULL };
const TokenDescr TOKEN_DESCR_DIR_BYTE			= { "BYTE", TOKEN_CLASS_DIRECTIVE, TOKEN_VALUE_TYPE_NONE, NULL };
const TokenDescr TOKEN_DESCR_DIR_GLOBAL			= { "GLOBAL", TOKEN_CLASS_DIRECTIVE, TOKEN_VALUE_TYPE_NONE, NULL };
const TokenDescr TOKEN_DESCR_DIR_ORG			= { "ORG", TOKEN_CLASS_DIRECTIVE, TOKEN_VALUE_TYPE_NONE, NULL };

//punctuation
const TokenDescr TOKEN_DESCR_COMMA				= { ",", TOKEN_CLASS_PUNCTUATION, TOKEN_VALUE_TYPE_NONE, NULL };
const TokenDescr TOKEN_DESCR_EOL				= { "EOL", TOKEN_CLASS_PUNCTUATION, TOKEN_VALUE_TYPE_NONE, NULL };
const TokenDescr TOKEN_DESCR_EOF				= { "EOF", TOKEN_CLASS_PUNCTUATION, TOKEN_VALUE_TYPE_NONE, NULL };

//instruction descriptors
const InstructionDescr INSTR_DESCR_ADD			= { 0x01, OPERAND_0 | OPERAND_1 | OPERAND_2 };
const InstructionDescr INSTR_DESCR_SUB			= { 0x02, OPERAND_0 | OPERAND_1 | OPERAND_2 };
const InstructionDescr INSTR_DESCR_OR			= { 0x03, OPERAND_0 | OPERAND_1 | OPERAND_2 };
const InstructionDescr INSTR_DESCR_XOR			= { 0x04, OPERAND_0 | OPERAND_1 | OPERAND_2 };
const InstructionDescr INSTR_DESCR_AND			= { 0x05, OPERAND_0 | OPERAND_1 | OPERAND_2 };
const InstructionDescr INSTR_DESCR_MOV_WIDE		= { 0x10, OPERAND_0 | OPERAND_3 };
const InstructionDescr INSTR_DESCR_MOV			= { 0x06, OPERAND_0 | OPERAND_1 };
const InstructionDescr INSTR_DESCR_MOV_EQ		= { 0x07, OPERAND_0 | OPERAND_1 };
const InstructionDescr INSTR_DESCR_MOV_NEQ		= { 0x08, OPERAND_0 | OPERAND_1 };
const InstructionDescr INSTR_DESCR_MOV_LESS		= { 0x09, OPERAND_0 | OPERAND_1 };
const InstructionDescr INSTR_DESCR_MOV_LESS_EQ	= { 0x0A, OPERAND_0 | OPERAND_1 };
const InstructionDescr INSTR_DESCR_CMP			= { 0x0B, OPERAND_1 | OPERAND_2 };
const InstructionDescr INSTR_DESCR_LDR			= { 0x0C, OPERAND_0 | OPERAND_1 };
const InstructionDescr INSTR_DESCR_STR			= { 0x0D, OPERAND_0 | OPERAND_1 };
const InstructionDescr INSTR_DESCR_CALL			= { 0x0E, OPERAND_1 };
