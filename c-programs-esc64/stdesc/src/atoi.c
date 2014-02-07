#include <stdesc.h>

int atoi(const char* str)
{
	int r = 0;
	int is_negative = 0;
	while(*str != '\0')
	{
		//search for first non-whitespace character
		 if(*str <= '9' && *str >= '0') {
			break;
		} else if (*str == '+') {
			str++;
			break;
		} else if (*str == '-') {
			is_negative = 1;
			str++;
			break;
		} else if(
			*str != ' ' && 
			*str != '\t' && 
			*str != '\n' && 
			*str != '\r' && 
			*str != '\v' && 
			*str != '\f') {
			
			return 0;
		}
		str++;
	}
	
	while(*str <= '9' && *str >= '0')
	{
		//multiply by 10
		r = (r << 3) + (r << 1);
		r += *str - '0';
		str++;
	}
	
	if(is_negative)	{ r = -r; }

	return r;
}
