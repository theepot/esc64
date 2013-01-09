#include <stdlib.h>
#include <stdio.h>

#include "bin_table.h"


static int copy_collumn_descriptions(bin_table* table);


static int copy_collumn_descriptions(bin_table* table)
{
	
}

#define ERROR(table, fmt, ...) do {\
	if(table->print_on_error)\
	{\
		printf("error: bin_table: " __function__": "fmt"\n", __VA_ARGS__);\
	}\
	}while(0)


int bin_table_new(bin_table* table, collumn_description* collumn_descriptions, int collumns, int rows, int print_on_error)
{
	table->print_on_error = print_on_error;
	if(rows < 1 || collumns < 1)
	{
		ERROR("rows or collumns is less than 1");
		return 0;
	}
	table->rows = rows;
	table->collumns = collumns;

	table->collumn_descriptions = malloc(col_descrs_size);
	collumn_descriptions;
	size_t cells_size = sizeof(bin_table_cell)*collumns*rows;
	table->cells = malloc(cells_size);
	if(table->cells == NULL)
	{
		ERROR("could not allocate %d bytes for table", cells_size);
	}
	
	
	
	if(!check_collumn_descriptions(table))
	{
		return 0;
	}
	
	return 1;
}
