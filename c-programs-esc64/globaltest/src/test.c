typedef char			int8_t;
typedef unsigned char	uint8_t;
typedef int				int16_t;
typedef unsigned		uint16_t;

#define INT8_MAX		(127)
#define INT16_MAX		(32767)
#define UINT8_MAX		(255)
#define UINT16_MAX		(65535)

#define INT8_MIN		(-128)
#define INT16_MIN		(-32767-1)

extern uint16_t A_uint16_t_aone2b;
extern uint16_t B_uint16_t_aone2b;

int main(int argc, char** argv)
{
	A_uint16_t_aone2b = 1;
	B_uint16_t_aone2b = A_uint16_t_aone2b;
	
	return 0;
}

