#include "link.h"

#include <stdio.h>
#include <assert.h>

#include "ioutils.h"
#include "objcode.h"

void ExeWriterInit(ExeWriter* writer, const char* path)
{
	writer->stream = fopen(path, "rb");
	assert(writer->stream);
}

void ExeWriterClose(ExeWriter* writer)
{
	fclose(writer->stream);
}

void ExeWriteBss(ExeWriter* writer, UWord_t address, UWord_t size)
{
	IOWriteByte(writer->stream, SECTION_TYPE_BSS);	//type
	IOWriteWord(writer->stream, address);			//address
	IOWriteWord(writer->stream, size);				//size
}

void ExeWriteData(ExeWriter* writer, UWord_t address, UWord_t size, const void* data)
{
	IOWriteByte(writer->stream, SECTION_TYPE_BSS);	//type
	IOWriteWord(writer->stream, address);			//address
	IOWriteWord(writer->stream, size);				//size
	IOWrite(writer->stream, data, size);			//data
}

void ExeReaderInit(ExeReader* reader, const char* path)
{
	reader->stream = fopen(path, "rb");
	assert(reader->stream);
	
	reader->type = SECTION_TYPE_NONE;
}

void ExeReaderClose(ExeReader* reader)
{
	fclose(reader->stream);
}

int ExeReadNext(ExeReader* reader)
{
#ifdef ESC_DEBUG
	if(reader->type == SECTION_TYPE_DATA)
	{
		assert(reader->dataRead);
	}
#endif

	reader->type = IOReadByte(reader->stream);		//type
	if(feof(reader->stream))
	{
		reader->type = SECTION_TYPE_NONE;
		return -1;
	}
	
#ifdef ESC_DEBUG
	if(reader->type == SECTION_TYPE_DATA)
	{
		reader->dataRead = 0;
	}
#endif

	reader->address = IOReadWord(reader->stream);	//address
	reader->size = IOReadWord(reader->stream);		//size
	
	return 0;
}

void ExeReadData(ExeReader* reader, void* data)
{
#ifdef ESC_DEBUG
	assert(reader->type == SECTION_TYPE_DATA);
	reader->dataRead = 1;
#endif
	if(!data)
	{
		//TODO delegate to ioutils
		assert(!fseek(reader->stream, reader->size, SEEK_CUR));
		return;
	}

	IORead(reader->stream, data, reader->size);
}

