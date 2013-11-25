#include <esc64asm/keywords.h>
#include <esc64asm/esctypes.h>
#include <esc64asm/escerror.h>
#include <esc64asm/scanner.h> //for quickfix

#include <esc64asm/keywords_gen.h>

int StrCmpNoCase(const char* a, const char* b)
{
	return gperf_case_strcmp(a, b);
}

const Keyword* FindKeyword(const PString* pStr)
{
	//FIXME quickfix, gperf want a null-character
	ESC_ASSERT_FATAL(pStr->size < SCANNER_BUF_SIZE, "FindKeyword(): Can't insert null-character for quickfix");
	((char*)pStr->str)[pStr->size] = 0;
	return FindKeyword_(pStr->str, pStr->size);
}

DirectiveHandler GetDirectiveHandler(byte_t index)
{
#ifdef ESC_DEBUG
	ESC_ASSERT_FATAL(
			index < (sizeof DIRECTIVE_HANDLERS / sizeof (DirectiveHandler)),
			"GetDirectiveHandler(): illegal index");
#endif
	return DIRECTIVE_HANDLERS[index];
}
