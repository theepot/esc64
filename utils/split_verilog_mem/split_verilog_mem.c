#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>

void print_usage(const char* const arg0)
{
	printf("Usage:\n\t%s (width filename )+\n", arg0);
}

typedef struct {
	FILE* file;
	int width;
} output;

int main(int argc, char** argv)
{
	if(argc == 1 || (argc-1) % 2 != 0)
	{
		print_usage(argv[0]);
		exit(1);
	}
	
	const int outputs_amount = (argc - 1) / 2;
	output outputs[outputs_amount];
	for(int n = 0; n < outputs_amount; n++)
	{
		char* filename = argv[n*2 + 2];
		char* width = argv[n*2 + 1];
		printf("output %s. width: %d\n", filename, atoi(width));
		outputs[n].width = atoi(width);
		outputs[n].file = fopen(filename, "w");
		if(outputs[n].file == NULL)
		{
			printf("Could not open file %s. Reason: %s\n", filename, strerror(errno));
			exit(1);
		}
	}

	char c;
	
	int index = 0;
	int output_n = 0;
	int in_comment = 0;
	while((c = getchar()) != EOF)
	{
		if(c == '\n')
		{
			index = output_n = in_comment = 0;
			for(int n = 0; n < outputs_amount; n++)
				fputc(c, outputs[n].file);
			continue;
		}
		if(!in_comment)
		{
			if(c == '/' || c == '@')
			{
				for(int n = 0; n < outputs_amount; n++)
					fputc(c, outputs[n].file);
				in_comment = 1;
				continue;
			}
		
			fputc(c, outputs[output_n].file);
			
			if((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
				index++;
			if(index == outputs[output_n].width && output_n != (outputs_amount-1))
			{
				output_n++;
				index = 0;
			}
		}
		else
		{
			for(int n = 0; n < outputs_amount; n++)
				fputc(c, outputs[n].file);
		}
	}
	
	for(int n = 0; n < outputs_amount; n++)
		fclose(outputs[n].file);
		
	return 0;
}
