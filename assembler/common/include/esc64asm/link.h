#ifndef LINK_INCLUDED
#define LINK_INCLUDED

#include <stdio.h>

#include <esc64asm/esctypes.h>

//	executable structure
//		- section[]

//	executable section structure
//		- type		: Byte_t
//		- address	: UWord_t
//		- size		: UWord_t
//		<<if type = data>>
//		- data		: <size>
//		<<end type = data>>
#define EXE_SECTION_TYPE_OFFSET	0
#define EXE_SECTION_ADDR_OFFSET	(EXE_SECTION_TYPE_OFFSET + sizeof (Byte_t))
#define EXE_SECTION_SIZE_OFFSET	(EXE_SECTION_ADDR_OFFSET + sizeof (UWord_t))
#define EXE_SECTION_DATA_OFFSET	(EXE_SECTION_SIZE_OFFSET + sizeof (UWord_t))

/**
 * @brief			Used to write sections to an executable
 */
typedef struct ExeWriter_
{
	FILE* stream;
} ExeWriter;

/**
 * @brief			Initializes the executable writer
 * @param path		Path to executable. File will be overwritten or created
 */
void ExeWriterInit(ExeWriter* writer, const char* path);

/**
 * @brief			Closes executable writer
 */
void ExeWriterClose(ExeWriter* writer);

/**
 * @brief			Write a BSS section
 * @param address	Address of BSS section
 * @param size		Size of BSS section
 */
void ExeWriteBss(ExeWriter* writer, UWord_t address, UWord_t size);

/**
 * @brief			Write a data section
 * @param address	Address of data section
 * @param size		Size of data section
 * @param data		Pointer to raw data in data section
 * @return			Offset of data in executable file
 */
ObjSize_t ExeWriteData(ExeWriter* writer, UWord_t address, UWord_t size, const void* data);

/**
 * @brief				Sets word [dataOffset + address*2] to value
 * @param dataOffset 	Offset of data
 * @param address		offset from dataOffset in words
 */
void ExeUpdateDataWord(ExeWriter* writer, ObjSize_t dataOffset, UWord_t address, UWord_t value);

/**
 * @brief			Describes ExeReader state
 */
typedef enum ExeReaderState_
{
	EXE_READER_STATE_START,			///< Reader is at the start of a section
	EXE_READER_STATE_READ_STATIC	///< Reader read the static part of the section
} ExeReaderState;

/**
 * @brief			Reader structure. Used to read executable files
 */
typedef struct ExeReader_
{
	FILE* stream;			///< Input stream
	Byte_t type;			///< Type of current section
	UWord_t address;		///< Address of current section
	UWord_t size;			///< Size of current section
	ExeReaderState state;	///< State of the reader
} ExeReader;

/**
 * @brief			Opens an executable file for reading
 * @param reader	Reader structure to be initialized
 * @param path		Path to executable
 */
void ExeReaderInit(ExeReader* reader, const char* path);

/**
 * @brief			Closes an executable reader.
 * @param reader	Reader structure to be closed
 */
void ExeReaderClose(ExeReader* reader);

/**
 * @brief			Read next section in executable
 *
 * If a data section is read, ExeReadData() *MUST* be invoked to read the data
 *
 * @param reader	Reader structure
 * @return			Non-zero if EOF is reached, Zero otherwise
 */
int ExeReadNext(ExeReader* reader);

/**
 * @brief			Read data of data section
 *
 * Should not be invoked if the current section is not a data section
 *
 * @param reader	Reader structure
 * @param data		Buffer to read data into. If NULL, data is discarded
 */
void ExeReadData(ExeReader* reader, void* data);

typedef struct SectionLinkInfo_
{
	UWord_t address;
	UWord_t size;
} SectionLinkInfo;

void Link(const char* exeName, const char** objFiles, size_t objFileCount);

#endif