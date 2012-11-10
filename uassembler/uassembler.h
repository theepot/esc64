#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#define L 0
#define H 1

#define NEXT_SEL_UCODE 0
#define NEXT_SEL_OPCODE 1

static int pow_(int n, int exp)
{
	if(exp == 0) return 1;
	
	int x = n;
	int i;
	for(i = 1; i < exp; ++i)
	{
		x *= n;
	}
	
	return x;
}

#define pow pow_

typedef struct {
	char* name;
	int index;
	int width;
	int active;
} field_description;

int compare_field_descriptions(const void* a, const void* b)
{
	return ((field_description*)a)->index - ((field_description*)b)->index;
}

typedef enum next_sel{
	fetch,
	next,
	op_entry
} next_sel;

typedef enum flag_condition{
	false,
	true,
	dontcare
} flag_condition;

field_description* field_descriptions;
int fields_amount = 0;
int addr_width = 0;
int data_width = 0;
int opcode_width = 0;
char* mem = 0;
int mem_size = 0;
int fetch_index = 0;
int next_uop_index = 0;

void initialise_field_descriptions()
{

	qsort(field_descriptions, fields_amount, sizeof(field_description), compare_field_descriptions);

	int i;
	int index = 0;
	for(i = 0; i < fields_amount; i++)
		index += field_descriptions[i].width;
	
	if(index != data_width)
	{
		fprintf(stderr, "error: sum of field width's does not equal data_width");
		exit(1);
	}
}

void print_mem_verilog_bin(const char* const m, const int size, const int comments)
{
	int i;
	if(comments)
	{
		printf("//");
		for(i = fields_amount - 1; i >= 0; i--)
		{
			printf("%s:%d", field_descriptions[i].name, field_descriptions[i].width);
			if(i != 0)
				putchar(' ');
		}
		putchar('\n');
	}
	
	int bit = 0;
	int field = fields_amount - 1;
	for(i = 0; i < size; i++)
	{
		assert(field >= 0);
		
		switch(m[i])
		{
			case 0: putchar('0'); break;
			case 1: putchar('1'); break;
			case 2: putchar('x'); break;
			default: fprintf(stderr, "error: encountered unkown bit code while printing. At uop number %d\n", i % data_width); exit(1); break;
		}
		
		bit++;
		if(field_descriptions[field].width == bit)
		{
			if(field != 0)
				putchar('_');
			bit = 0;
			field--;

		}
		
		if((i+1) % data_width == 0)
		{
			printf(" //%d\n", i / data_width);
			field = fields_amount - 1;
			bit = 0;
		}
	}
}

int find_field_by_name(const char* const name)
{
	int i;
	for(i = 0; i < fields_amount; i++)
	{
		if(!strcmp(name, field_descriptions[i].name))
			return i;
	}
	
	return -1;
}

char* strip(char* string)
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

void change_field(const int field_number, const int value, char* const mem, const int uopn)
{
	if(field_number >= fields_amount)
	{
		fprintf(stderr, "error in function change_field: field wiht number %d does not exist. At uop number %d\n", field_number, uopn);
		exit(1);
	}
	if(pow(2, field_descriptions[field_number].width) - 1 < value)
	{
		fprintf(stderr, "warning: putting value %d in a field with width %d. Truncating bits. At uop number %d\n", value, field_descriptions[field_number].width, uopn);
	}
	int i;
	for(i = 0; i < field_descriptions[field_number].width; i++)
	{
		mem[(uopn * data_width) + data_width - field_descriptions[field_number].index - i - 1] = (value >> i) & 1;
	}
}

void turn_all_fields_inactive(char* const mem, const int uopn)
{
	int i;
	for(i = 0; i < fields_amount; i++)
	{
		if(field_descriptions[i].active)
			change_field(i, 0, mem, uopn);
		else
			change_field(i, ((1 << field_descriptions[i].width) - 1), mem, uopn);
	}
}

void change_fields_by_string(const char* const fieldvalues, char* const mem, const int uopn)
{
	const int len = strlen(fieldvalues);
	if(len == 0)
		return;
	
	const char* start;
	const char* end = fieldvalues;
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
			char* tmp2 = strip(tmp);
			int field_number = find_field_by_name(tmp2);
			if(field_number == -1)
			{
				fprintf(stderr, "error: unkown field name %s at uop number %d\n", tmp, uopn);
				exit(1);
			}
			if(field_descriptions[field_number].width != 1)
			{
				fprintf(stderr, "error: specified multibit value without explicit assignment. At uop number %d\n", uopn);
			}
			change_field(field_number, field_descriptions[field_number].active, mem, uopn);
		}
		else
		{
			char tmp[sign-start+1];
			memcpy(tmp, start, sign-start);
			tmp[sign-start] = '\0';
			char* tmp2 = strip(tmp);
			int field_number = find_field_by_name(tmp2);
			if(field_number == -1)
			{
				fprintf(stderr, "error: unkown field name %s at uop number %d\n", tmp, uopn);
				exit(1);
			}
			char tmp3[end-sign];
			memcpy(tmp3, sign+1, end-sign-1);
			tmp3[end-sign-1] = '\0';
			char* tmp4 = strip(tmp3);
			int value = atoi(tmp4);
			change_field(field_number, value, mem, uopn);
		}
		
		if(end-fieldvalues == len)
		{
			break;
		}
		else
		{
			end++;
		}
	}
	
	
}

void set_uop(const char* const fieldvalues, int next, int nextsel, char* const mem, int uopn)
{
		turn_all_fields_inactive(mem, uopn);
		change_fields_by_string(fieldvalues, mem, uopn);
		change_field(find_field_by_name("next"), next, mem, uopn);
		change_field(find_field_by_name("nextsel"), nextsel, mem, uopn);
}

void put_op_entry(const char* const fieldvalues, int opcode, flag_condition c, flag_condition z, const next_sel nxt, char* mem)
{

	int nxt_addr = 0;
	int nxt_sel = NEXT_SEL_UCODE;
	switch(nxt)
	{
		case fetch: nxt_addr = fetch_index; break;
		case next: nxt_addr = next_uop_index; break;
		case op_entry: nxt_sel = NEXT_SEL_OPCODE; break;
		default:
			fprintf(stderr, "error: unkown next_sel\n");
			exit(1);
		break;
	}
	//notzero notcarry
	if((c == dontcare || c == false) && (z == dontcare || z == false))
		set_uop(fieldvalues, nxt_addr, nxt_sel, mem, opcode << 2);
	
	//notzero carry
	if((c == dontcare || c == true) && (z == dontcare || z == false))
		set_uop(fieldvalues, nxt_addr, nxt_sel, mem, (opcode << 2) | 2);

	//zero notcarry
	if((c == dontcare || c == false) && (z == dontcare || z == true))
		set_uop(fieldvalues, nxt_addr, nxt_sel, mem, (opcode << 2) | 1);
	
	//zero carry
	if((c == dontcare || c == true) && (z == dontcare || z == true))
		set_uop(fieldvalues, nxt_addr, nxt_sel, mem, (opcode << 2) | 3);
	
}

void put_uop(const char* const fieldvalues, const next_sel nxt, char* mem)
{
	switch(nxt)
	{
		case fetch:
			set_uop(fieldvalues, fetch_index, NEXT_SEL_UCODE, mem, next_uop_index);
		break;
		case next:
			set_uop(fieldvalues, next_uop_index + 1, NEXT_SEL_UCODE, mem, next_uop_index);
		break;
		case op_entry:
			set_uop(fieldvalues, 0, NEXT_SEL_OPCODE, mem, next_uop_index);
		break;
		default:
			fprintf(stderr, "error: unkown next_sel\n");
			exit(1);
		break;
	}
	
	next_uop_index++;
}

void init()
{
	//init
	initialise_field_descriptions();
	mem_size = pow(2, addr_width) * data_width;
	mem = malloc(mem_size);
	memset(mem, 2, mem_size);
}
