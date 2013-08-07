#include <esc64asm/objread.h>

#include <string.h>
#include <errno.h>
#include <esc64asm/ioutils.h>
#include <esc64asm/escerror.h>
#include <esc64asm/link.h>

#define POSTFIX_STACK_SIZE	32

static FILE* objStream_;
static byte_t secType_;			///< type of current section
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
		EscError("%s", strerror(errno));
	}

	secNext_ = OBJ_RECORD_ILLEGAL_OFFSET;
	IOSeekForward(objStream_, OBJ_HEADER_SIZE);
}

void ObjReadHeader(ObjectHeader* header)
{
	//TODO check if this is even necessary
	objsize_t head = IOGetFilePos(objStream_);
	IOSetFilePos(objStream_, 0);

	header->localSymTotNameSize = IOReadWord(objStream_);
	header->globalSymTotNameSize = IOReadWord(objStream_);
	header->localSymCount = IOReadWord(objStream_);
	header->globalSymCount = IOReadWord(objStream_);
	header->absSectionCount = IOReadWord(objStream_);
	header->relocSectionCount = IOReadWord(objStream_);
	header->absSectionOffset = IOReadObjSize(objStream_);
	header->relocSectionOffset = IOReadObjSize(objStream_);

	//TODO and this
	IOSetFilePos(objStream_, head);
}

void ObjectReaderStart(objsize_t firstOffset)
{
	secNext_ = firstOffset;
}

void ObjReadSection(objsize_t offset)
{
	IOSetFilePos(objStream_, offset);
	secType_ = IOReadByte(objStream_);		//type
	secNext_ = IOReadObjSize(objStream_);	//next

	secOffset_ = offset;
}

void ObjectReaderClose(void)
{
	fclose(objStream_);
}

int ObjReaderNextSection(void)
{
	if(secNext_ == OBJ_RECORD_ILLEGAL_OFFSET)
	{
		return -1;
	}

	ObjReadSection(secNext_);
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

uword_t ObjReadAddress(void)
{
	IOSetFilePos(
			objStream_,
			secOffset_ + OBJ_SECTION_ADDRESS_OFFSET);
	return IOReadWord(objStream_);
}


uword_t ObjReadSize(void)
{
	IOSetFilePos(
				objStream_,
				secOffset_ + OBJ_SECTION_SIZE_OFFSET);
	return IOReadWord(objStream_);
}

byte_t ObjReadType(void)
{
	IOSetFilePos(
			objStream_,
			secOffset_ + OBJ_SECTION_TYPE_OFFSET);
	return IOReadByte(objStream_);
}

byte_t ObjGetType(void)
{
	return secType_;
}

objsize_t ObjGetSectionOffset(void)
{
	return secOffset_;
}

objsize_t ObjGetSectionNext(void)
{
	return secNext_;
}

objsize_t ObjGetDataOffset(void)
{
	return secOffset_ + OBJ_SECTION_DATA_RECORD_OFFSET;
}

int ObjSymIteratorInit(ObjSymIterator* it, objsize_t offset)
{
	it->stream = objStream_;
	IOSetFilePos(it->stream, secOffset_ + offset);
	objsize_t recordOffset = IOReadObjSize(it->stream);
	if(recordOffset == OBJ_RECORD_ILLEGAL_OFFSET)
	{
		return -1;
	}

	RecordReaderInit(&it->symReader, objStream_, recordOffset);
	it->state = OBJ_IT_STATE_START;

	return 0;
}

int ObjSymIteratorNext(ObjSymIterator* it)
{
	ESC_ASSERT_FATAL(it->state == OBJ_IT_STATE_START, "ObjSymIterator state != START");

	size_t toRead = sizeof (uword_t) + sizeof (uword_t);
	byte_t buf[toRead];
	void* p = buf;
	if(RecordRead(&it->symReader, it->stream, buf, toRead) != toRead)
	{
		return -1;
	}

	uword_t rawValue;
	memcpy(&rawValue, p, sizeof rawValue);
	p += sizeof rawValue;
	it->curSym.address = NTOH_WORD(rawValue);			//value

	uword_t rawNameSize;
	memcpy(&rawNameSize, p, sizeof rawNameSize);
	it->curSym.nameLen = NTOH_WORD(rawNameSize);	//nameSize

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

int ObjExpReaderInit(ObjExpReader* expReader)
{
	IOSetFilePos(objStream_, secOffset_ + OBJ_SECTION_EXPR_RECORD_OFFSET);
	objsize_t firstOffset = IOReadObjSize(objStream_);
	if(firstOffset == OBJ_RECORD_ILLEGAL_OFFSET)
	{
		return -1;
	}

	RecordReaderInit(&expReader->reader, objStream_, firstOffset);
	return 0;
}

int ObjExpReaderNext(ObjExpReader* expReader, uword_t* address, uword_t* value)
{
	if(RecordReadWord(&expReader->reader, objStream_, address))
	{
		return -1;
	}

	postfixStackN_ = 0;

	for(;;) //while type != EXPR_T_END
	{
		byte_t type;
		uword_t result;
		ESC_ASSERT_FATAL(!RecordReadByte(&expReader->reader, objStream_, &type), "Unexpected start or end to postfix expression");

		switch(type)
		{
		case EXPR_T_OP_AND:
			result = PostfixPeek(1) & PostfixPeek(0);
			PostfixPop(2);
			break;
		case EXPR_T_OP_PLUS:
			result = PostfixPeek(1) + PostfixPeek(0);
			PostfixPop(2);
			break;
		case EXPR_T_OP_OR:
			result = PostfixPeek(1) | PostfixPeek(0);
			PostfixPop(2);
			break;
		case EXPR_T_OP_NOT:
			result = ~PostfixPeek(0);
			PostfixPop(1);
			break;
		case EXPR_T_OP_SUB:
			result = PostfixPeek(1) - PostfixPeek(0);
			PostfixPop(2);
			break;
		case EXPR_T_OP_NEG:
			result = -PostfixPeek(0);
			PostfixPop(1);
			break;
		case EXPR_T_OP_DIVIDE:
			result = PostfixPeek(1) / PostfixPeek(0);
			PostfixPop(2);
			break;
		case EXPR_T_WORD:
			ESC_ASSERT_FATAL(!RecordReadWord(&expReader->reader, objStream_, &result), "Incomplete postfix token with type EXPR_T_WORD");
			break;
		case EXPR_T_SYMBOL:
			{
				uword_t size;
				ESC_ASSERT_FATAL(!RecordReadWord(&expReader->reader, objStream_, &size), "Incomplete postfix token with type EXPR_T_SYMBOL (1)");
				char str[size];
				ESC_ASSERT_FATAL(RecordRead(&expReader->reader, objStream_, str, size) == size, "Incomplete postfix token with type EXPR_T_SYMBOL (2)");
				uword_t value;
				if(ResolveSymbol(str, size, &value))
				{
					EscError("Unable to resolve symbol `%s'", str);
				}
				result = value;
			} break;
		case EXPR_T_END:
			goto EndLoop; //break from loop

		default:
			EscFatal("Unknown postfix expresion token type (0x%X) in object reader", type);
			break; //prevent warning
		}

		PostfixPush(result);
	} EndLoop:

	ESC_ASSERT_FATAL(postfixStackN_ == 1, "too many values on postfix stack");
	*value = postfixStack_[0];

	return 0;
}

int ObjDataReaderInit(ObjDataReader* dataReader)
{
	assert(secType_ == SECTION_TYPE_DATA);

	dataReader->stream = objStream_;
	IOSetFilePos(dataReader->stream, secOffset_ + OBJ_SECTION_DATA_RECORD_OFFSET);
	objsize_t firstOffset = IOReadObjSize(dataReader->stream);
	if(firstOffset == OBJ_RECORD_ILLEGAL_OFFSET)
	{
		return -1;
	}

	RecordReaderInit(&dataReader->dataReader, dataReader->stream, firstOffset);

	return 0;
}

size_t ObjDataReaderRead(ObjDataReader* reader, void* data, size_t dataSize)
{
	return RecordRead(&reader->dataReader, objStream_, data, dataSize * 2);
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





