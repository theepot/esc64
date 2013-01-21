#ifndef LINK_INCLUDED
#define LINK_INCLUDED

#include <stdio.h>

#include "esctypes.h"

//	executable section structure
//		- type		: Byte_t
//		- address	: UWord_t
//		- size		: UWord_t
//		<<if type = data>>
//		- data
//		<<end type = data>>
#define EXE_SECTION_TYPE_OFFSET	0
#define EXE_SECTION_ADDR_OFFSET	(EXE_SECTION_TYPE_OFFSET + sizeof (Byte_t))
#define EXE_SECTION_SIZE_OFFSET	(EXE_SECTION_ADDR_OFFSET + sizeof (UWord_t))
#define EXE_SECTION_DATA_OFFSET	(EXE_SECTION_SIZE_OFFSET + sizeof (UWord_t))

typedef struct ExeWriter_
{
	FILE* stream;
} ExeWriter;

void ExeWriterInit(ExeWriter* writer, const char* path);
void ExeWriterClose(ExeWriter* writer);

void ExeWriteBss(ExeWriter* writer, UWord_t address, UWord_t size);
void ExeWriteData(ExeWriter* writer, UWord_t address, UWord_t size, const void* data);

/**
 * @brief			Reader structure. Used to read executable files
 */
typedef struct ExeReader_
{
	FILE* stream;		///< Input stream
	Byte_t type;		///< Type of current section
	UWord_t address;	///< Address of current section
	UWord_t size;		///< Size of current section
#ifdef ESC_DEBUG
	int readData;
#endif
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

#endif
