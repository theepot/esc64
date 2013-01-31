#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "objcode.h"
#include "link.h"
#include "esctypes.h"

#define MAX_TOTAL_SIZE		(1 << 15)
#define NO_TEST_SECTIONS	64
#define MAX_SECTION_SIZE	(MAX_TOTAL_SIZE / NO_TEST_SECTIONS)
#define TYPE_DATA			0
#define TYPE_BSS			1

static UWord_t Section_n_to_size(int section)
{
	UWord_t result  = (section * 54321) % MAX_SECTION_SIZE;
	if(result == 0)
	{
		result += 100;
	}

	return result;
}

static int Section_n_to_type(int section)
{
	return (section * 13521) % 1;
}

static void Section_n_to_data(int section, uint8_t* data, UWord_t size)
{
	int n;
	for(n = 0; n < size; ++n)
	{
		data[n] = (section*n*12341) % 0xFF;
	}
}

static void write_test_sections(ExeWriter* writerPtr, int* out_section_addresses)
{
	int section;
	for(section = 0; section < NO_TEST_SECTIONS; ++section)
	{
		out_section_addresses[section] = MAX_SECTION_SIZE * section;

		UWord_t address = out_section_addresses[section];
		UWord_t size = Section_n_to_size(section);
		int type = Section_n_to_type(section);
		if(type == TYPE_DATA)
		{
			//create raw data
			uint8_t data[size];
			Section_n_to_data(section, data, size);
			ExeWriteData(writerPtr, address, size, (void*)data);
		}
		else if(type == TYPE_BSS)
		{
			ExeWriteBss(writerPtr, address, size);
		}
		else
		{
			assert(0);
		}
	}
}

int section_n_by_address(const int* section_addresses, int address)
{
	int n;
	for(n = 0; n < NO_TEST_SECTIONS; ++n)
	{
		if(section_addresses[n] == address)
			return n;
	}

	return -1;
}

static void read_compare_sections(ExeReader* readerPtr, const int* section_addresses)
{
	int found_sections[NO_TEST_SECTIONS];
	memset(found_sections, 0, sizeof(found_sections));

	int no_sections_read = 0;
	while(!ExeReadNext(readerPtr))
	{
		++no_sections_read;
		int section_n = section_n_by_address(section_addresses, readerPtr->address);
		assert(section_n != -1);
		UWord_t size = readerPtr->size;
		assert(Section_n_to_size(section_n) == size);
		if(readerPtr->type == SECTION_TYPE_DATA)
		{
			assert(Section_n_to_type(section_n) == TYPE_DATA);
			uint8_t data_read[size];
			uint8_t data_shouldbe[size];
			ExeReadData(readerPtr, data_read);
			Section_n_to_data(section_n, data_shouldbe, size);
			assert(memcmp(data_read, data_shouldbe, size) == 0);
		}
		else if(readerPtr->type == SECTION_TYPE_BSS)
		{
			assert(Section_n_to_type(section_n) == TYPE_BSS);
		}
		else
		{
			assert(0);
		}
		assert(!found_sections[section_n]);
		found_sections[section_n] = 1;
	}
	assert(no_sections_read == NO_TEST_SECTIONS);
}

void TestExecWriteRead(const char* file)
{
	//list with section addresses by section number
	int section_addresses[NO_TEST_SECTIONS];

	//create an ExeWriter
	ExeWriter writer;
	ExeWriter* writerPtr = &writer;
	ExeWriterInit(writerPtr, file);

	//write stuff
	write_test_sections(writerPtr, section_addresses);

	//destruct the ExeWriter
	ExeWriterClose(writerPtr);

	//create an ExeReader
	ExeReader reader;
	ExeReader* readerPtr = &reader;
	ExeReaderInit(readerPtr, file);

	//read & compare stuff
	read_compare_sections(readerPtr, section_addresses);

	//destruct the ExeReader
	ExeReaderClose(readerPtr);

}
