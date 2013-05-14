#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "bin_table.h"


#define ERROR(table, fmt, ...) do {\
	if(table->print_on_error)\
	{\
		fprintf(stderr, "ERROR: bin_table: "fmt"\n", ##__VA_ARGS__);\
	}\
	if(table->exit_on_error)\
	{\
		exit(1);\
	}\
	} while(0)


//static forward declarations
static void initialize_table(bin_table* table);
static char* strip_string(char* string);


static char* strip_string(char* string)
{
	int len = strlen(string);
	if(len < 1)
		return 0;

	char* start = string;
	while(isblank(*start))
	{
		start++;
	}

	char* end = string + len - 1;

	while(isblank(*end) && end != string)
	{
		end--;
	}
	end++;

	*end = '\0';
	return start;
}

static void initialize_table(bin_table* table)
{
	int n;
	for(n = 0; n < table->collumns*table->rows; ++n)
	{
		table->cells[n].is_known = 0;
	}
}

int bin_table_new(bin_table* table, bin_table_collumn_description* collumn_descriptions, int collumns, int rows, int max_total_width, int print_on_error, int exit_on_error)
{
	table->print_on_error = print_on_error;
	table->exit_on_error = exit_on_error;
	if(rows < 1 || collumns < 1)
	{
		ERROR(table, "rows or collumns is less than 1");
		return 0;
	}
	table->rows = rows;
	table->collumns = collumns;

	size_t col_descrs_size = collumns*sizeof(bin_table_collumn_description);
	table->collumn_descriptions = malloc(col_descrs_size);
	memcpy(table->collumn_descriptions, collumn_descriptions, col_descrs_size);

	size_t cells_size = sizeof(bin_table_cell)*collumns*rows;
	table->cells = malloc(cells_size);
	if(table->cells == NULL)
	{
		ERROR(table, "could not allocate %zu bytes for table", cells_size); return 0;
	}
	
	if(max_total_width < 0)
	{
		ERROR(table, "max_total_width can not be less than zero. zero means no maximum."); return 0;
	}

	int n;
	int padding_collumn = -1;
	int total_width = 0;
	for(n = 0; n < collumns; ++n)
	{
		if(table->collumn_descriptions[n].width == -1)
		{
			if(padding_collumn == -1)
			{
				if(max_total_width != 0)
				{
					padding_collumn = n;
				}
				else
				{
					ERROR(table, "table has padding collumn but no max total width is specified"); return 0;
				}
			}
			else
			{
				ERROR(table, "table can only have one padding collumn"); return 0;
			}
		}
		else if(table->collumn_descriptions[n].width <= 0)
		{
			ERROR(table, "width of collumn can only be -1 of greater than zero"); return 0;
		}
		else
		{
			total_width += table->collumn_descriptions[n].width;
		}

		if(total_width > max_total_width)
		{
			ERROR(table, "total width of collumns is greater than the maximum total width"); return 0;
		}
	}

	if(padding_collumn != -1)
	{
		table->collumn_descriptions[padding_collumn].width = max_total_width - total_width;
	}

	//set_collumn_indices(collumn_descriptions, collumns);
	initialize_table(table);
	
	return 1;
}

void bin_table_free(bin_table* table)
{
	free(table->cells);
	free(table->collumn_descriptions);
}

int bin_table_set_cell_value(bin_table* table, int collumn_index, int row, int value)
{
	if(row >= table->rows)
	{
		ERROR(table, "row > number of rows in table"); return 0;
	}

	if(collumn_index >= table->collumns)
	{
		ERROR(table, "collumn_index > number of collumns in table"); return 0;
	}
	if(value >= (1 << table->collumn_descriptions[collumn_index].width))
	{
		ERROR(table, "value %d does not fit in collumn %s with width %d", value,
				table->collumn_descriptions[collumn_index].name,
				table->collumn_descriptions[collumn_index].width);
		return 0;
	}

	table->cells[row*table->collumns + collumn_index] = (bin_table_cell){.value = value, .is_known = 1};

	return 1;
}

int bin_table_set_row_inactive(bin_table* table, int row)
{
	int n;
	for(n = 0; n < table->collumns; ++n)
	{
		if(table->collumn_descriptions[n].active_high)
		{
			if(!bin_table_set_cell_value(table, n, row, 0))
				return 0;
		}
		else
		{
			if(!bin_table_set_cell_value(table, n, row, (1 << table->collumn_descriptions[n].width) - 1))
				return 0;
		}
	}
	return 1;
}

int bin_table_get_cell_value(bin_table* table, int collumn_index, int row, int* out_val)
{
	if(row >= table->rows)
	{
		ERROR(table, "row > number of rows in table"); return 0;
	}

	if(collumn_index >= table->collumns)
	{
		ERROR(table, "collumn_index > number of collumns in table"); return 0;
	}

	if(!table->cells[row*table->collumns + collumn_index].is_known)
		return 0;
	*out_val = table->cells[row*table->collumns + collumn_index].value;

	return 1;
}

int bin_table_copy_row(bin_table* table, int dest, int src)
{
	if(dest >= table->rows)
	{
		ERROR(table, "dest row > number of rows"); return 0;
	}

	if(src >= table->rows)
	{
		ERROR(table, "src row > number of rows"); return 0;
	}

	memcpy(&table->cells[dest*table->collumns], &table->cells[src*table->collumns], sizeof(bin_table_cell) * table->collumns);

	return 1;
}

int bin_table_collumn_by_name(bin_table* table, const char* name)
{
	int i;
	for(i = 0; i < table->collumns; i++)
	{
		if(!strcmp(name, table->collumn_descriptions[i].name))
			return i;
	}

	ERROR(table, "unkown collumn name %s.", name);
	return -1;
}

void bin_table_print_binverilog(bin_table* table, FILE* f, int comments)
{
	int i;
	if(comments)
	{
		fprintf(f, "//");
		for(i = 0; i < table->collumns; ++i)
		{
			fprintf(f, "%s:%d ", table->collumn_descriptions[i].name, table->collumn_descriptions[i].width);
		}
		fputc('\n', f);
	}

	int row, collumn, bit;
	for(row = 0; row < table->rows; ++row)
	{
		for(collumn = 0; collumn < table->collumns; ++collumn)
		{
			if(table->cells[row*table->collumns + collumn].is_known)
			{
				int value = table->cells[row*table->collumns + collumn].value;
				for(bit = 0; bit < table->collumn_descriptions[collumn].width; ++bit)
				{
					if(value & (1 << (table->collumn_descriptions[collumn].width - 1)))
						fputc('1', f);
					else
						fputc('0', f);
					value <<= 1;
				}
			}
			else
			{
				for(bit = 0; bit < table->collumn_descriptions[collumn].width; ++bit)
				{
					fputc('x', f);
				}
			}
			if(collumn != (table->collumns - 1))
			{
				fputc('_', f);
			}

		}
		if(comments)
			fprintf(f, " //%d\n", row);
	}
}
