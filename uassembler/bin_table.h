#ifndef __BIN_TABLE_H
#define __BIN_TABLE_H

#include <stdint.h>
#include <stdio.h>

typedef struct {
	char* name;
	int index;
	int width;
	int active_high;
} bin_table_collumn_description;

typedef struct {
	uint32 value;
	uint32 is_known;
} bin_table_cell;

typedef struct {
	collumn_description* collumn_descriptions;
	cell* cells;
	int collumns;
	int rows;
	int print_on_error;
} bin_table;

int bin_table_new(bin_table* table, collumn_description* collumn_descriptions, int collumns, int rows, int print_on_error);
int bin_table_free(bin_table* table);

int bin_table_set_cell_value(bin_table* table, int collumn_index, int row, int value);
int bin_table_get_cell_value(bin_table* table, int collumn_index, int row);
void bin_table_set_row_inactive(bin_table* table, int row);
int bin_table_change_row_by_string(bin_table* table, const char* cell_values, int row);
int bin_table_set_table_unkown(bin_table* table);

int bin_table_collumn_by_name(bin_table* table, const char* name);

void bin_table_print_binverilog(bin_table* table, FILE* f);



#endif
