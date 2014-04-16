#include <esc64asm/objread.h>

#include <string.h>
#include <errno.h>
#include <stddef.h>

#include <esc64asm/ioutils.h>
#include <esc64asm/escerror.h>
#include <esc64asm/link.h>

#define POSTFIX_STACK_SIZE	32

static FILE* objStream_;
//static byte_t secType_;			///< type of current section
static objsize_t secOffset_;	///< offset of current section
static objsize_t secNext_;		///< offset of next section
static word_t postfixStack_[POSTFIX_STACK_SIZE];
static size_t postfixStackN_;

static void PostfixPush(word_t val);
static word_t PostfixPeek(size_t depth);
static void PostfixPop(size_t amount);

void ObjectReaderInit(const char* path)
{
	objStream_ = fopen(path, "rb");
	if(!objStream_)
	{
		EscError("%s: %s", strerror(errno), path);
	}

	secNext_ = OBJ_RECORD_ILLEGAL_OFFSET;
	IOSeekForward(objStream_, sizeof (ObjHeader));
}

void ObjReadHeader(ObjHeader* header)
{
	objsize_t head = IOGetFilePos(objStream_);
	IOSetFilePos(objStream_, 0);

//	header->localSymTotNameSize = IOReadWord(objStream_);
//	header->globalSymTotNameSize = IOReadWord(objStream_);
//	header->localSymCount = IOReadWord(objStream_);
//	header->globalSymCount = IOReadWord(objStream_);
//	header->absSectionCount = IOReadWord(objStream_);
//	header->relocSectionCount = IOReadWord(objStream_);
//	header->absSectionOffset = IOReadObjSize(objStream_);
//	header->relocSectionOffset = IOReadObjSize(objStream_);
	IORead(objStream_, header, sizeof *header);

	IOSetFilePos(objStream_, head);
}

void ObjectReaderStart(objsize_t firstOffset)
{
	secNext_ = firstOffset;
}

void ObjReadSection(objsize_t offset, ObjSectionHeader* sectionHeader)
{
	secOffset_ = offset;
	IOSetFilePos(objStream_, secOffset_);
	IORead(objStream_, sectionHeader, sizeof *sectionHeader);
	secNext_ = betoh_objsize(sectionHeader->next);
}

void ObjectReaderClose(void)
{
	fclose(objStream_);
}

int ObjReaderNextSection(ObjSectionHeader* sectionHeader)
{
	if(secNext_ == OBJ_RECORD_ILLEGAL_OFFSET)
	{
		return -1;
	}

	ObjReadSection(secNext_, sectionHeader);
	return 0;

//	IOSetFilePos(stream_, offset_ + OBJ_SECTION_NEXT_OFFSET);
//	ObjSize_t next = IOReadObjSize(stream_);
//	if(next == OBJ_RECORD_ILLEGAL_OFFSET)
//	{
//		return -1;
//	}
//
//	ReadSection(reader, next);
//	return 0;
}

//uword_t ObjReadAddress(void)
//{
//	IOSetFilePos(
//			objStream_,
//			secOffset_ + OBJ_SECTION_ADDRESS_OFFSET);
//	return IOReadWord(objStream_);
//}
//
//
//uword_t ObjReadSize(void)
//{
//	IOSetFilePos(
//				objStream_,
//				secOffset_ + OBJ_SECTION_SIZE_OFFSET);
//	return IOReadWord(objStream_);
//}
//
//byte_t ObjReadType(void)
//{
//	IOSetFilePos(
//			objStream_,
//			secOffset_ + OBJ_SECTION_TYPE_OFFSET);
//	return IOReadByte(objStream_);
//}

//byte_t ObjGetType(void)
//{
//	return secType_;
//}

objsize_t ObjGetSectionOffset(void)
{
	return secOffset_;
}

objsize_t ObjGetSectionNext(void)
{
	return secNext_;
}

//objsize_t ObjGetDataOffset(void)
//{
//	return secOffset_ + OBJ_SECTION_DATA_RECORD_OFFSET;
//}

//TODO check all calls to ObjSimIteratorInit(), make sure offset now points to the offset of the record, NOT the offset of the offset of the record!
int ObjSymIteratorInit(ObjSymIterator* it, objsize_t offset)
{
	it->stream = objStream_;
//	IOSetFilePos(it->stream, secOffset_ + offset);
//	objsize_t recordOffset = IOReadObjSize(it->stream);
//	if(recordOffset == OBJ_RECORD_ILLEGAL_OFFSET)
//	{
//		return -1;
//	}

	//FIXME quickfix, should probably check for offset == OBJ_RECORD_ILLEGAL_OFFSET at callsites
	if(offset == OBJ_RECORD_ILLEGAL_OFFSET) { return -1; }

	RecordReaderInit(&it->symReader, objStream_, offset);
	it->state = OBJ_IT_STATE_START;

	return 0;
}

int ObjSymIteratorNext(ObjSymIterator* it)
{
	ESC_ASSERT_FATAL(it->state == OBJ_IT_STATE_START, "ObjSymIterator state != START");

	size_t toRead = sizeof (uword_t) + sizeof (uword_t);
	byte_t buf[toRead];
	byte_t* p = buf;
	if(RecordRead(&it->symReader, it->stream, buf, toRead) != toRead)
	{
		return -1;
	}

	uword_t rawValue;
	memcpy(&rawValue, p, sizeof rawValue);
	p += sizeof rawValue;
	it->curSym.address = betoh_word(rawValue);		//value

	uword_t rawNameSize;
	memcpy(&rawNameSize, p, sizeof rawNameSize);
	it->curSym.nameLen = betoh_word(rawNameSize);	//nameSize

	it->state = OBJ_IT_STATE_READ_STATIC;

	return 0;
}

void ObjSymIteratorReadName(ObjSymIterator* it, void* buf)
{
	assert(it->state == OBJ_IT_STATE_READ_STATIC);
	assert(RecordRead(&it->symReader, it->stream, buf, it->curSym.nameLen) == it->curSym.nameLen);
	it->curSym.name = buf;
	it->state = OBJ_IT_STATE_START;
}

const Symbol* ObjSymIteratorGetSym(ObjSymIterator* it)
{
	return &it->curSym;
}

int XObjExpReaderInit(XObjExpReader* reader)
{
//	IOSetFilePos(objStream_, secOffset_ + OBJ_SECTION_EXPR_RECORD_OFFSET);
	objsize_t exp = secOffset_ + sizeof (ObjSectionHeader) + offsetof(ObjDataSection, expRecordOffset);
	IOSetFilePos(objStream_, exp);

	objsize_t firstOffset = IOReadObjSizeBE(objStream_);

	if(firstOffset == OBJ_RECORD_ILLEGAL_OFFSET) { return -1; }

	RecordReaderInit(&reader->reader, objStream_, firstOffset);
	return 0;
}

int XObjExpReaderNextExp(XObjExpReader* reader)
{
	if(RecordReadWordBE(&reader->reader, objStream_, &reader->address)) { return -1; }
	return 0;
}

int XObjExpReaderNextToken(XObjExpReader* reader)
{
	ESC_ASSERT_ERROR(
			!RecordReadByte(&reader->reader, objStream_, &reader->tok.type),
			"Unexpected start or end to postfix expression");
	switch(reader->tok.type)
	{
	case EXPR_T_WORD:
	case EXPR_T_SYMBOL:
		ESC_ASSERT_ERROR(
				!RecordReadWordBE(&reader->reader, objStream_, (uword_t*)&reader->tok.wordVal),
				"Incomplete token in postfix expression");
		break;

	case EXPR_T_END:	return -1;
	}

	return 0;
}

int XObjExpReaderGetStr(XObjExpReader* reader)
{
#ifdef ESC_DEBUG
	ESC_ASSERT_FATAL(reader->tok.type == EXPR_T_SYMBOL, "XObjExpReaderGetStr() called while type != symbol");
#endif
	ESC_ASSERT_ERROR(
			RecordRead(&reader->reader, objStream_, reader->tok.strVal, reader->tok.strLen) == reader->tok.strLen,
			"Incomplete token in postfix expression");
	return 0;
}

int ObjExpReaderInit(ObjExpReader* expReader)
{
//	IOSetFilePos(objStream_, secOffset_ + OBJ_SECTION_EXPR_RECORD_OFFSET);
	objsize_t exp = secOffset_ + sizeof (ObjSectionHeader) + offsetof(ObjDataSection, expRecordOffset);
	IOSetFilePos(objStream_, exp);

	objsize_t firstOffset = IOReadObjSizeBE(objStream_);
	if(firstOffset == OBJ_RECORD_ILLEGAL_OFFSET)
	{
		return -1;
	}

	RecordReaderInit(&expReader->reader, objStream_, firstOffset);
	return 0;
}

int ObjExpReaderNext(ObjExpReader* expReader, uword_t* address, uword_t* value)
{
	if(RecordReadWordBE(&expReader->reader, objStream_, address))
	{
		return -1;
	}

	postfixStackN_ = 0;

	for(;;) //while type != EXPR_T_END
	{
		byte_t type;
		uword_t result;
		ESC_ASSERT_FATAL(!RecordReadByte(&expReader->reader, objStream_, &type), "Unexpected start or end to postfix expression");

#ifdef ESC_DEBUG
#define DUMP(s)	printf("ObjExpReaderNext(): type=%s, sp=%u", (s), postfixStackN_)
#define RES2	printf(", result=%d, with [%d, %d]\n", result, PostfixPeek(1), PostfixPeek(0))
#define RES1	printf(", result=%d, with [%d]\n", result, PostfixPeek(0))
#else
#define DUMP(s)
#define RES2
#define RES1
#endif

		switch(type)
		{
		case EXPR_T_OP_AND:
			DUMP("and");
			result = PostfixPeek(1) & PostfixPeek(0);
			RES2;
			PostfixPop(2);
			break;
		case EXPR_T_OP_PLUS:
			DUMP("plus");
			result = PostfixPeek(1) + PostfixPeek(0);
			RES2;
			PostfixPop(2);
			break;
		case EXPR_T_OP_OR:
			DUMP("or");
			result = PostfixPeek(1) | PostfixPeek(0);
			RES2;
			PostfixPop(2);
			break;
		case EXPR_T_OP_NOT:
			DUMP("not");
			result = ~PostfixPeek(0);
			RES1;
			PostfixPop(1);
			break;
		case EXPR_T_OP_SUB:
			DUMP("sub");
			result = PostfixPeek(1) - PostfixPeek(0);
			RES2;
			PostfixPop(2);
			break;
		case EXPR_T_OP_NEG:
			DUMP("neg");
			result = -PostfixPeek(0);
			RES1;
			PostfixPop(1);
			break;
		case EXPR_T_OP_DIV:
			DUMP("div");
			result = PostfixPeek(1) / PostfixPeek(0);
			RES2;
			PostfixPop(2);
			break;
		case EXPR_T_WORD:
			DUMP("word");
			ESC_ASSERT_FATAL(!RecordReadWordBE(&expReader->reader, objStream_, &result), "Incomplete postfix token with type EXPR_T_WORD");
#ifdef ESC_DEBUG
			printf(", result=%d\n", result);
#endif
			break;
		case EXPR_T_SYMBOL:
			{
				DUMP("symbol");
				uword_t size;
				ESC_ASSERT_FATAL(!RecordReadWordBE(&expReader->reader, objStream_, &size), "Incomplete postfix token with type EXPR_T_SYMBOL (1)");
				char str[size];
				ESC_ASSERT_FATAL(RecordRead(&expReader->reader, objStream_, str, size) == size, "Incomplete postfix token with type EXPR_T_SYMBOL (2)");
#ifdef ESC_DEBUG
				{
					printf(", str=");
					char* c;
					for(c = str; c < str + size; ++c) { putchar(*c); }
					putchar('\n');
				}
#endif
				uword_t value;
				if(ResolveSymbol(str, size, &value))
				{
					EscError("Unable to resolve symbol `%s'", str);
				}
				result = value;
			} break;
		case EXPR_T_END:
			DUMP("end");
			goto EndLoop; //break from loop

		default:
			EscFatal("Unknown postfix expresion token type (0x%X) in object reader", type);
			break; //prevent warning
		}

		PostfixPush(result);
	} EndLoop:

	ESC_ASSERT_FATAL(postfixStackN_ == 1, "too many values on postfix stack");
	*value = postfixStack_[0];

#ifdef ESC_DEBUG
	printf("ObjExpReaderNext(): *value=%d\n", *value);
#endif

	return 0;
}

int ObjDataReaderInit(ObjDataReader* dataReader)
{
	dataReader->stream = objStream_;
	objsize_t data = secOffset_ + sizeof (ObjSectionHeader) + offsetof(ObjDataSection, dataRecordOffset);
	IOSetFilePos(objStream_, data);

	objsize_t firstOffset = IOReadObjSizeBE(dataReader->stream);
	if(firstOffset == OBJ_RECORD_ILLEGAL_OFFSET)
	{
		return -1;
	}

	RecordReaderInit(&dataReader->dataReader, dataReader->stream, firstOffset);

	return 0;
}

size_t ObjDataReaderRead(ObjDataReader* reader, void* data, size_t dataSize)
{
	return RecordRead(&reader->dataReader, objStream_, data, dataSize);
}

static void PostfixPush(word_t val)
{
	ESC_ASSERT_FATAL(postfixStackN_ + 1 < POSTFIX_STACK_SIZE, "Postfix stack overflow");
	postfixStack_[postfixStackN_++] = val;
}

static word_t PostfixPeek(size_t depth)
{
	size_t n = postfixStackN_ - 1 - depth;
	ESC_ASSERT_FATAL(n < POSTFIX_STACK_SIZE, "PostfixPeek() Out of bounds");
	return postfixStack_[n];
}

static void PostfixPop(size_t amount)
{
	ESC_ASSERT_FATAL(postfixStackN_ - amount < POSTFIX_STACK_SIZE, "Postfix stack underflow");
	postfixStackN_ -= amount;
}





