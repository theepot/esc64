#ifndef __BIN_TABLE_H
#define __BIN_TABLE_H

#include <stdint.h>
#include <stdio.h>

typedef struct {
	char* name;
	int width;
	int active_high;

	//private
	int index;
} bin_table_collumn_description;

typedef struct {
	uint32_t value;
	uint32_t is_known;
} bin_table_cell;

typedef struct {
	bin_table_collumn_description* collumn_descriptions;
	bin_table_cell* cells;
	int collumns;
	int rows;
	int print_on_error;
	int exit_on_error;
} bin_table;

int bin_table_new(bin_table* table, bin_table_collumn_description* collumn_descriptions, int collumns, int rows, int print_on_error, int exit_on_error);
void bin_table_free(bin_table* table);

int bin_table_set_cell_value(bin_table* table, int collumn_index, int row, int value);
int bin_table_get_cell_value(bin_table* table, int collumn_index, int row, int* out_val);
int bin_table_change_row_by_string(bin_table* table, const char* cell_values, int row);
int bin_table_set_row_inactive(bin_table* table, int row);

int bin_table_copy_row(bin_table* table, int dest, int src);

int bin_table_collumn_by_name(bin_table* table, const char* name);

void bin_table_print_binverilog(bin_table* table, FILE* f, int comments);



#endif
