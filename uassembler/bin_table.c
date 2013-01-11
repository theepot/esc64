#include <stdlib.h>
#include <stdio.h>

#include "bin_table.h"


#define ERROR(table, fmt, ...) do {\
	if(table->print_on_error)\
	{\
		fprintf(stderr, "error: bin_table: " __function__": "fmt"\n", __VA_ARGS__);\
	}\
	if(table->exit_on_error)\
	{\
		exit(1);
	}\
	}while(0)

//static forward declarations
static void set_collumn_indices(bin_table_collumn_description* collumn_descriptions, int collumns);
static void initialize_table(bin_table* table);
static char* strip_string(char* string);

static void set_collumn_indices(bin_table_collumn_description* collumn_descriptions, int collumns)
{
	int i;
	int index = 0;
	for(i = 0; i < collumns; i++)
	{
		collumn_descriptions[i].index = index;
		index += collumn_descriptions[i].width;
	}

}

static char* strip_string(char* string)
{
	int len = strlen(string);
	if(len < 1)
		return 0;

	char* start = string;
	while(isblank(*start)){
		start++;
	}

	char* end = string + len - 1;

	while(isblank(*end) && end != string){
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

int bin_table_new(bin_table* table, bin_table_collumn_description* collumn_descriptions, int collumns, int rows, int print_on_error, int exit_on_error)
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
		ERROR("could not allocate %d bytes for table", cells_size); return 0;
	}
	
	set_collumn_indices(collumn_descriptions, collumns);
	initialize_table(table);
	
	return 1;
}

void bin_table_free(bin_table* table)
{
	free(table->cells);
	free(table->collumn_descriptions);
}

void bin_table_set_cell_value(bin_table* table, int collumn_index, int row, int value)
{
	if(row >= table->rows)
	{
		ERROR(table, "row > number of rows in table"); return;
	}

	if(collumn_index >= table->collumns)
	{
		ERROR(table, "collumn_index > number of collumns in table"); return;
	}
	if(value >= (1 << table->collumn_descriptions[collumn_index].width))
	{
		ERROR(table, "value %d does not fit in collumn %s with width %d", value,
				table->collumn_descriptions[collumn_index].name,
				table->collumn_descriptions[collumn_index].width);
		return;
	}

	table->cells[row*table->collumns + collumn_index] = (bin_table_cell){.value = value, .is_known = 1};

}

int bin_table_get_cell_value(bin_table* table, int collumn_index, int row)
{
	if(row >= table->rows)
	{
		ERROR(table, "row > number of rows in table"); return 0;
	}

	if(collumn_index >= table->collumns)
	{
		ERROR(table, "collumn_index > number of collumns in table"); return 0;
	}

	if(table->cells[row*table->collumns + collumn_index].is_known)
		return table->cells[row*table->collumns + collumn_index].value;
	else
		return -1;
}

void bin_table_change_row_by_string(bin_table* table, const char* cell_values, int row)
{
	const int len = strlen(cell_values);
	if(len == 0)
		return;

	const char* start;
	const char* end = cell_values;
	const char* sign = 0;
	for(;;)
	{
		sign = 0;
		start = end;
		while(*end != ',' && *end != '\0')
		{
			if(*end == '=')
				sign = end;
			end++;
		}

		if(sign == 0)
		{
			char tmp[end-start+1];
			memcpy(tmp, start, end-start);
			tmp[end-start] = '\0';
			char* tmp2 = strip_string(tmp);
			int collumn_number = find_field_by_name(tmp2);
			if(collumn_number == -1)
			{
				exit(1);
			}
			if(table->collumn_descriptions[collumn_number].width != 1)
			{
				ERROR("specified multibit value without explicit assignment.");
			}
			//change_field(field_number, table->collumn_descriptions[field_number].active_high, row);
			bin_table_set_cell_value(table, collumn_number, row, table->collumn_descriptions[collumn_number].active_high);
		}
		else
		{
			char tmp[sign-start+1];
			memcpy(tmp, start, sign-start);
			tmp[sign-start] = '\0';
			char* tmp2 = strip_string(tmp);
			int field_number = bin_table_collumn_by_name(table, tmp2);
			if(field_number == -1)
			{
				exit(1);
			}
			char tmp3[end-sign];
			memcpy(tmp3, sign+1, end-sign-1);
			tmp3[end-sign-1] = '\0';
			char* tmp4 = strip_string(tmp3);
			int value = atoi(tmp4);
			//change_field(field_number, value, row);
			bin_table_set_cell_value(table, field_number, row, value);
		}

		if(end-cell_values == len)
		{
			break;
		}
		else
		{
			end++;
		}
	}

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
		for(i = table->collumns - 1; i >= 0; i--)
		{
			fprintf(f, "%s:%d", table->collumn_descriptions[i].name, table->collumn_descriptions[i].width);
			if(i != 0)
				putchar(' ');
		}
		fputc(f, '\n');
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
					if(value & (1 << table->collumn_descriptions[collumn].width - 1))
						fputc(f, '1');
					else
						fputc(f, '0');
					value <<= 1;
				}
			}
			else
			{
				for(bit = 0; bit < table->collumn_descriptions[collumn].width; ++bit)
				{
					fputc(f, 'x');
				}
			}
			if(collumn != (table->collumns - 1))
			{
				fputc(f, '_');
			}

		}
		if(comments)
			fprintf(f, " //%d\n", row);
	}
}
