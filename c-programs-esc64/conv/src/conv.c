#include <esctest.h>

static int8_t A_int8_t_amax2b1;
static int8_t B_int8_t_amax2b1;
static int8_t A_int8_t_amin2b1;
static int8_t B_int8_t_amin2b1;
static int8_t A_int8_t_azero2b1;
static int8_t B_int8_t_azero2b1;
static int8_t A_int8_t_aone2b1;
static int8_t B_int8_t_aone2b1;
static int8_t A_int8_t_aMinusOne2b1;
static int8_t B_int8_t_aMinusOne2b1;
static int8_t A_int8_t_amax2b2;
static uint8_t B_uint8_t_amax2b2;
static int8_t A_int8_t_amin2b2;
static uint8_t B_uint8_t_amin2b2;
static int8_t A_int8_t_azero2b2;
static uint8_t B_uint8_t_azero2b2;
static int8_t A_int8_t_aone2b2;
static uint8_t B_uint8_t_aone2b2;
static int8_t A_int8_t_aMinusOne2b2;
static uint8_t B_uint8_t_aMinusOne2b2;
static int8_t A_int8_t_amax2b3;
static int16_t B_int16_t_amax2b3;
static int8_t A_int8_t_amin2b3;
static int16_t B_int16_t_amin2b3;
static int8_t A_int8_t_azero2b3;
static int16_t B_int16_t_azero2b3;
static int8_t A_int8_t_aone2b3;
static int16_t B_int16_t_aone2b3;
static int8_t A_int8_t_aMinusOne2b3;
static int16_t B_int16_t_aMinusOne2b3;
static int8_t A_int8_t_amax2b4;
static uint16_t B_uint16_t_amax2b4;
static int8_t A_int8_t_amin2b4;
static uint16_t B_uint16_t_amin2b4;
static int8_t A_int8_t_azero2b4;
static uint16_t B_uint16_t_azero2b4;
static int8_t A_int8_t_aone2b4;
static uint16_t B_uint16_t_aone2b4;
static int8_t A_int8_t_aMinusOne2b4;
static uint16_t B_uint16_t_aMinusOne2b4;
static uint8_t A_uint8_t_amax2b5;
static int8_t B_int8_t_amax2b5;
static uint8_t A_uint8_t_amin2b5;
static int8_t B_int8_t_amin2b5;
static uint8_t A_uint8_t_azero2b5;
static int8_t B_int8_t_azero2b5;
static uint8_t A_uint8_t_aone2b5;
static int8_t B_int8_t_aone2b5;
static uint8_t A_uint8_t_amax2b6;
static uint8_t B_uint8_t_amax2b6;
static uint8_t A_uint8_t_amin2b6;
static uint8_t B_uint8_t_amin2b6;
static uint8_t A_uint8_t_azero2b6;
static uint8_t B_uint8_t_azero2b6;
static uint8_t A_uint8_t_aone2b6;
static uint8_t B_uint8_t_aone2b6;
static uint8_t A_uint8_t_amax2b7;
static int16_t B_int16_t_amax2b7;
static uint8_t A_uint8_t_amin2b7;
static int16_t B_int16_t_amin2b7;
static uint8_t A_uint8_t_azero2b7;
static int16_t B_int16_t_azero2b7;
static uint8_t A_uint8_t_aone2b7;
static int16_t B_int16_t_aone2b7;
static uint8_t A_uint8_t_amax2b8;
static uint16_t B_uint16_t_amax2b8;
static uint8_t A_uint8_t_amin2b8;
static uint16_t B_uint16_t_amin2b8;
static uint8_t A_uint8_t_azero2b8;
static uint16_t B_uint16_t_azero2b8;
static uint8_t A_uint8_t_aone2b8;
static uint16_t B_uint16_t_aone2b8;
static int16_t A_int16_t_amax2b9;
static int8_t B_int8_t_amax2b9;
static int16_t A_int16_t_amin2b9;
static int8_t B_int8_t_amin2b9;
static int16_t A_int16_t_azero2b9;
static int8_t B_int8_t_azero2b9;
static int16_t A_int16_t_aone2b9;
static int8_t B_int8_t_aone2b9;
static int16_t A_int16_t_aMinusOne2b9;
static int8_t B_int8_t_aMinusOne2b9;
static int16_t A_int16_t_amax2b10;
static uint8_t B_uint8_t_amax2b10;
static int16_t A_int16_t_amin2b10;
static uint8_t B_uint8_t_amin2b10;
static int16_t A_int16_t_azero2b10;
static uint8_t B_uint8_t_azero2b10;
static int16_t A_int16_t_aone2b10;
static uint8_t B_uint8_t_aone2b10;
static int16_t A_int16_t_aMinusOne2b10;
static uint8_t B_uint8_t_aMinusOne2b10;
static int16_t A_int16_t_amax2b11;
static int16_t B_int16_t_amax2b11;
static int16_t A_int16_t_amin2b11;
static int16_t B_int16_t_amin2b11;
static int16_t A_int16_t_azero2b11;
static int16_t B_int16_t_azero2b11;
static int16_t A_int16_t_aone2b11;
static int16_t B_int16_t_aone2b11;
static int16_t A_int16_t_aMinusOne2b11;
static int16_t B_int16_t_aMinusOne2b11;
static int16_t A_int16_t_amax2b12;
static uint16_t B_uint16_t_amax2b12;
static int16_t A_int16_t_amin2b12;
static uint16_t B_uint16_t_amin2b12;
static int16_t A_int16_t_azero2b12;
static uint16_t B_uint16_t_azero2b12;
static int16_t A_int16_t_aone2b12;
static uint16_t B_uint16_t_aone2b12;
static int16_t A_int16_t_aMinusOne2b12;
static uint16_t B_uint16_t_aMinusOne2b12;
static uint16_t A_uint16_t_amax2b13;
static int8_t B_int8_t_amax2b13;
static uint16_t A_uint16_t_amin2b13;
static int8_t B_int8_t_amin2b13;
static uint16_t A_uint16_t_azero2b13;
static int8_t B_int8_t_azero2b13;
static uint16_t A_uint16_t_aone2b13;
static int8_t B_int8_t_aone2b13;
static uint16_t A_uint16_t_amax2b14;
static uint8_t B_uint8_t_amax2b14;
static uint16_t A_uint16_t_amin2b14;
static uint8_t B_uint8_t_amin2b14;
static uint16_t A_uint16_t_azero2b14;
static uint8_t B_uint8_t_azero2b14;
static uint16_t A_uint16_t_aone2b14;
static uint8_t B_uint8_t_aone2b14;
static uint16_t A_uint16_t_amax2b15;
static int16_t B_int16_t_amax2b15;
static uint16_t A_uint16_t_amin2b15;
static int16_t B_int16_t_amin2b15;
static uint16_t A_uint16_t_azero2b15;
static int16_t B_int16_t_azero2b15;
static uint16_t A_uint16_t_aone2b15;
static int16_t B_int16_t_aone2b15;
static uint16_t A_uint16_t_amax2b16;
static uint16_t B_uint16_t_amax2b16;
static uint16_t A_uint16_t_amin2b16;
static uint16_t B_uint16_t_amin2b16;
static uint16_t A_uint16_t_azero2b16;
static uint16_t B_uint16_t_azero2b16;
static uint16_t A_uint16_t_aone2b16;
static uint16_t B_uint16_t_aone2b16;

int main(int argc, char** argv)
{
	A_int8_t_amax2b1 = INT8_MAX;
	B_int8_t_amax2b1 = A_int8_t_amax2b1;
	A_int8_t_amin2b1 = INT8_MIN;
	B_int8_t_amin2b1 = A_int8_t_amin2b1;
	A_int8_t_azero2b1 = 0;
	B_int8_t_azero2b1 = A_int8_t_azero2b1;
	A_int8_t_aone2b1 = 1;
	B_int8_t_aone2b1 = A_int8_t_aone2b1;
	A_int8_t_aMinusOne2b1 = -1;
	B_int8_t_aMinusOne2b1 = A_int8_t_aMinusOne2b1;

	A_int8_t_amax2b2 = INT8_MAX;
	B_uint8_t_amax2b2 = A_int8_t_amax2b2;
	A_int8_t_amin2b2 = INT8_MIN;
	B_uint8_t_amin2b2 = A_int8_t_amin2b2;
	A_int8_t_azero2b2 = 0;
	B_uint8_t_azero2b2 = A_int8_t_azero2b2;
	A_int8_t_aone2b2 = 1;
	B_uint8_t_aone2b2 = A_int8_t_aone2b2;
	A_int8_t_aMinusOne2b2 = -1;
	B_uint8_t_aMinusOne2b2 = A_int8_t_aMinusOne2b2;

	A_int8_t_amax2b3 = INT8_MAX;
	B_int16_t_amax2b3 = A_int8_t_amax2b3;
	A_int8_t_amin2b3 = INT8_MIN;
	B_int16_t_amin2b3 = A_int8_t_amin2b3;
	A_int8_t_azero2b3 = 0;
	B_int16_t_azero2b3 = A_int8_t_azero2b3;
	A_int8_t_aone2b3 = 1;
	B_int16_t_aone2b3 = A_int8_t_aone2b3;
	A_int8_t_aMinusOne2b3 = -1;
	B_int16_t_aMinusOne2b3 = A_int8_t_aMinusOne2b3;

	A_int8_t_amax2b4 = INT8_MAX;
	B_uint16_t_amax2b4 = A_int8_t_amax2b4;
	A_int8_t_amin2b4 = INT8_MIN;
	B_uint16_t_amin2b4 = A_int8_t_amin2b4;
	A_int8_t_azero2b4 = 0;
	B_uint16_t_azero2b4 = A_int8_t_azero2b4;
	A_int8_t_aone2b4 = 1;
	B_uint16_t_aone2b4 = A_int8_t_aone2b4;
	A_int8_t_aMinusOne2b4 = -1;
	B_uint16_t_aMinusOne2b4 = A_int8_t_aMinusOne2b4;

	A_uint8_t_amax2b5 = UINT8_MAX;
	B_int8_t_amax2b5 = A_uint8_t_amax2b5;
	A_uint8_t_amin2b5 = 0;
	B_int8_t_amin2b5 = A_uint8_t_amin2b5;
	A_uint8_t_azero2b5 = 0;
	B_int8_t_azero2b5 = A_uint8_t_azero2b5;
	A_uint8_t_aone2b5 = 1;
	B_int8_t_aone2b5 = A_uint8_t_aone2b5;

	A_uint8_t_amax2b6 = UINT8_MAX;
	B_uint8_t_amax2b6 = A_uint8_t_amax2b6;
	A_uint8_t_amin2b6 = 0;
	B_uint8_t_amin2b6 = A_uint8_t_amin2b6;
	A_uint8_t_azero2b6 = 0;
	B_uint8_t_azero2b6 = A_uint8_t_azero2b6;
	A_uint8_t_aone2b6 = 1;
	B_uint8_t_aone2b6 = A_uint8_t_aone2b6;

	A_uint8_t_amax2b7 = UINT8_MAX;
	B_int16_t_amax2b7 = A_uint8_t_amax2b7;
	A_uint8_t_amin2b7 = 0;
	B_int16_t_amin2b7 = A_uint8_t_amin2b7;
	A_uint8_t_azero2b7 = 0;
	B_int16_t_azero2b7 = A_uint8_t_azero2b7;
	A_uint8_t_aone2b7 = 1;
	B_int16_t_aone2b7 = A_uint8_t_aone2b7;

	A_uint8_t_amax2b8 = UINT8_MAX;
	B_uint16_t_amax2b8 = A_uint8_t_amax2b8;
	A_uint8_t_amin2b8 = 0;
	B_uint16_t_amin2b8 = A_uint8_t_amin2b8;
	A_uint8_t_azero2b8 = 0;
	B_uint16_t_azero2b8 = A_uint8_t_azero2b8;
	A_uint8_t_aone2b8 = 1;
	B_uint16_t_aone2b8 = A_uint8_t_aone2b8;

	A_int16_t_amax2b9 = INT16_MAX;
	B_int8_t_amax2b9 = A_int16_t_amax2b9;
	A_int16_t_amin2b9 = INT16_MIN;
	B_int8_t_amin2b9 = A_int16_t_amin2b9;
	A_int16_t_azero2b9 = 0;
	B_int8_t_azero2b9 = A_int16_t_azero2b9;
	A_int16_t_aone2b9 = 1;
	B_int8_t_aone2b9 = A_int16_t_aone2b9;
	A_int16_t_aMinusOne2b9 = -1;
	B_int8_t_aMinusOne2b9 = A_int16_t_aMinusOne2b9;

	A_int16_t_amax2b10 = INT16_MAX;
	B_uint8_t_amax2b10 = A_int16_t_amax2b10;
	A_int16_t_amin2b10 = INT16_MIN;
	B_uint8_t_amin2b10 = A_int16_t_amin2b10;
	A_int16_t_azero2b10 = 0;
	B_uint8_t_azero2b10 = A_int16_t_azero2b10;
	A_int16_t_aone2b10 = 1;
	B_uint8_t_aone2b10 = A_int16_t_aone2b10;
	A_int16_t_aMinusOne2b10 = -1;
	B_uint8_t_aMinusOne2b10 = A_int16_t_aMinusOne2b10;

	A_int16_t_amax2b11 = INT16_MAX;
	B_int16_t_amax2b11 = A_int16_t_amax2b11;
	A_int16_t_amin2b11 = INT16_MIN;
	B_int16_t_amin2b11 = A_int16_t_amin2b11;
	A_int16_t_azero2b11 = 0;
	B_int16_t_azero2b11 = A_int16_t_azero2b11;
	A_int16_t_aone2b11 = 1;
	B_int16_t_aone2b11 = A_int16_t_aone2b11;
	A_int16_t_aMinusOne2b11 = -1;
	B_int16_t_aMinusOne2b11 = A_int16_t_aMinusOne2b11;

	A_int16_t_amax2b12 = INT16_MAX;
	B_uint16_t_amax2b12 = A_int16_t_amax2b12;
	A_int16_t_amin2b12 = INT16_MIN;
	B_uint16_t_amin2b12 = A_int16_t_amin2b12;
	A_int16_t_azero2b12 = 0;
	B_uint16_t_azero2b12 = A_int16_t_azero2b12;
	A_int16_t_aone2b12 = 1;
	B_uint16_t_aone2b12 = A_int16_t_aone2b12;
	A_int16_t_aMinusOne2b12 = -1;
	B_uint16_t_aMinusOne2b12 = A_int16_t_aMinusOne2b12;

	A_uint16_t_amax2b13 = UINT16_MAX;
	B_int8_t_amax2b13 = A_uint16_t_amax2b13;
	A_uint16_t_amin2b13 = 0;
	B_int8_t_amin2b13 = A_uint16_t_amin2b13;
	A_uint16_t_azero2b13 = 0;
	B_int8_t_azero2b13 = A_uint16_t_azero2b13;
	A_uint16_t_aone2b13 = 1;
	B_int8_t_aone2b13 = A_uint16_t_aone2b13;

	A_uint16_t_amax2b14 = UINT16_MAX;
	B_uint8_t_amax2b14 = A_uint16_t_amax2b14;
	A_uint16_t_amin2b14 = 0;
	B_uint8_t_amin2b14 = A_uint16_t_amin2b14;
	A_uint16_t_azero2b14 = 0;
	B_uint8_t_azero2b14 = A_uint16_t_azero2b14;
	A_uint16_t_aone2b14 = 1;
	B_uint8_t_aone2b14 = A_uint16_t_aone2b14;

	A_uint16_t_amax2b15 = UINT16_MAX;
	B_int16_t_amax2b15 = A_uint16_t_amax2b15;
	A_uint16_t_amin2b15 = 0;
	B_int16_t_amin2b15 = A_uint16_t_amin2b15;
	A_uint16_t_azero2b15 = 0;
	B_int16_t_azero2b15 = A_uint16_t_azero2b15;
	A_uint16_t_aone2b15 = 1;
	B_int16_t_aone2b15 = A_uint16_t_aone2b15;

	A_uint16_t_amax2b16 = UINT16_MAX;
	B_uint16_t_amax2b16 = A_uint16_t_amax2b16;
	A_uint16_t_amin2b16 = 0;
	B_uint16_t_amin2b16 = A_uint16_t_amin2b16;
	A_uint16_t_azero2b16 = 0;
	B_uint16_t_azero2b16 = A_uint16_t_azero2b16;
	A_uint16_t_aone2b16 = 1;
	B_uint16_t_aone2b16 = A_uint16_t_aone2b16;

	//print
	print_str("A_int8_t_amax2b1=");
	print_hex16(A_int8_t_amax2b1);
	print_char('\n');
	print_str("B_int8_t_amax2b1=");
	print_hex16(B_int8_t_amax2b1);
	print_char('\n');
	print_str("A_int8_t_amin2b1=");
	print_hex16(A_int8_t_amin2b1);
	print_char('\n');
	print_str("B_int8_t_amin2b1=");
	print_hex16(B_int8_t_amin2b1);
	print_char('\n');
	print_str("A_int8_t_azero2b1=");
	print_hex16(A_int8_t_azero2b1);
	print_char('\n');
	print_str("B_int8_t_azero2b1=");
	print_hex16(B_int8_t_azero2b1);
	print_char('\n');
	print_str("A_int8_t_aone2b1=");
	print_hex16(A_int8_t_aone2b1);
	print_char('\n');
	print_str("B_int8_t_aone2b1=");
	print_hex16(B_int8_t_aone2b1);
	print_char('\n');
	print_str("A_int8_t_aMinusOne2b1=");
	print_hex16(A_int8_t_aMinusOne2b1);
	print_char('\n');
	print_str("B_int8_t_aMinusOne2b1=");
	print_hex16(B_int8_t_aMinusOne2b1);
	print_char('\n');
	print_str("A_int8_t_amax2b2=");
	print_hex16(A_int8_t_amax2b2);
	print_char('\n');
	print_str("B_uint8_t_amax2b2=");
	print_hex16(B_uint8_t_amax2b2);
	print_char('\n');
	print_str("A_int8_t_amin2b2=");
	print_hex16(A_int8_t_amin2b2);
	print_char('\n');
	print_str("B_uint8_t_amin2b2=");
	print_hex16(B_uint8_t_amin2b2);
	print_char('\n');
	print_str("A_int8_t_azero2b2=");
	print_hex16(A_int8_t_azero2b2);
	print_char('\n');
	print_str("B_uint8_t_azero2b2=");
	print_hex16(B_uint8_t_azero2b2);
	print_char('\n');
	print_str("A_int8_t_aone2b2=");
	print_hex16(A_int8_t_aone2b2);
	print_char('\n');
	print_str("B_uint8_t_aone2b2=");
	print_hex16(B_uint8_t_aone2b2);
	print_char('\n');
	print_str("A_int8_t_aMinusOne2b2=");
	print_hex16(A_int8_t_aMinusOne2b2);
	print_char('\n');
	print_str("B_uint8_t_aMinusOne2b2=");
	print_hex16(B_uint8_t_aMinusOne2b2);
	print_char('\n');
	print_str("A_int8_t_amax2b3=");
	print_hex16(A_int8_t_amax2b3);
	print_char('\n');
	print_str("B_int16_t_amax2b3=");
	print_hex16(B_int16_t_amax2b3);
	print_char('\n');
	print_str("A_int8_t_amin2b3=");
	print_hex16(A_int8_t_amin2b3);
	print_char('\n');
	print_str("B_int16_t_amin2b3=");
	print_hex16(B_int16_t_amin2b3);
	print_char('\n');
	print_str("A_int8_t_azero2b3=");
	print_hex16(A_int8_t_azero2b3);
	print_char('\n');
	print_str("B_int16_t_azero2b3=");
	print_hex16(B_int16_t_azero2b3);
	print_char('\n');
	print_str("A_int8_t_aone2b3=");
	print_hex16(A_int8_t_aone2b3);
	print_char('\n');
	print_str("B_int16_t_aone2b3=");
	print_hex16(B_int16_t_aone2b3);
	print_char('\n');
	print_str("A_int8_t_aMinusOne2b3=");
	print_hex16(A_int8_t_aMinusOne2b3);
	print_char('\n');
	print_str("B_int16_t_aMinusOne2b3=");
	print_hex16(B_int16_t_aMinusOne2b3);
	print_char('\n');
	print_str("A_int8_t_amax2b4=");
	print_hex16(A_int8_t_amax2b4);
	print_char('\n');
	print_str("B_uint16_t_amax2b4=");
	print_hex16(B_uint16_t_amax2b4);
	print_char('\n');
	print_str("A_int8_t_amin2b4=");
	print_hex16(A_int8_t_amin2b4);
	print_char('\n');
	print_str("B_uint16_t_amin2b4=");
	print_hex16(B_uint16_t_amin2b4);
	print_char('\n');
	print_str("A_int8_t_azero2b4=");
	print_hex16(A_int8_t_azero2b4);
	print_char('\n');
	print_str("B_uint16_t_azero2b4=");
	print_hex16(B_uint16_t_azero2b4);
	print_char('\n');
	print_str("A_int8_t_aone2b4=");
	print_hex16(A_int8_t_aone2b4);
	print_char('\n');
	print_str("B_uint16_t_aone2b4=");
	print_hex16(B_uint16_t_aone2b4);
	print_char('\n');
	print_str("A_int8_t_aMinusOne2b4=");
	print_hex16(A_int8_t_aMinusOne2b4);
	print_char('\n');
	print_str("B_uint16_t_aMinusOne2b4=");
	print_hex16(B_uint16_t_aMinusOne2b4);
	print_char('\n');
	print_str("A_uint8_t_amax2b5=");
	print_hex16(A_uint8_t_amax2b5);
	print_char('\n');
	print_str("B_int8_t_amax2b5=");
	print_hex16(B_int8_t_amax2b5);
	print_char('\n');
	print_str("A_uint8_t_amin2b5=");
	print_hex16(A_uint8_t_amin2b5);
	print_char('\n');
	print_str("B_int8_t_amin2b5=");
	print_hex16(B_int8_t_amin2b5);
	print_char('\n');
	print_str("A_uint8_t_azero2b5=");
	print_hex16(A_uint8_t_azero2b5);
	print_char('\n');
	print_str("B_int8_t_azero2b5=");
	print_hex16(B_int8_t_azero2b5);
	print_char('\n');
	print_str("A_uint8_t_aone2b5=");
	print_hex16(A_uint8_t_aone2b5);
	print_char('\n');
	print_str("B_int8_t_aone2b5=");
	print_hex16(B_int8_t_aone2b5);
	print_char('\n');
	print_str("A_uint8_t_amax2b6=");
	print_hex16(A_uint8_t_amax2b6);
	print_char('\n');
	print_str("B_uint8_t_amax2b6=");
	print_hex16(B_uint8_t_amax2b6);
	print_char('\n');
	print_str("A_uint8_t_amin2b6=");
	print_hex16(A_uint8_t_amin2b6);
	print_char('\n');
	print_str("B_uint8_t_amin2b6=");
	print_hex16(B_uint8_t_amin2b6);
	print_char('\n');
	print_str("A_uint8_t_azero2b6=");
	print_hex16(A_uint8_t_azero2b6);
	print_char('\n');
	print_str("B_uint8_t_azero2b6=");
	print_hex16(B_uint8_t_azero2b6);
	print_char('\n');
	print_str("A_uint8_t_aone2b6=");
	print_hex16(A_uint8_t_aone2b6);
	print_char('\n');
	print_str("B_uint8_t_aone2b6=");
	print_hex16(B_uint8_t_aone2b6);
	print_char('\n');
	print_str("A_uint8_t_amax2b7=");
	print_hex16(A_uint8_t_amax2b7);
	print_char('\n');
	print_str("B_int16_t_amax2b7=");
	print_hex16(B_int16_t_amax2b7);
	print_char('\n');
	print_str("A_uint8_t_amin2b7=");
	print_hex16(A_uint8_t_amin2b7);
	print_char('\n');
	print_str("B_int16_t_amin2b7=");
	print_hex16(B_int16_t_amin2b7);
	print_char('\n');
	print_str("A_uint8_t_azero2b7=");
	print_hex16(A_uint8_t_azero2b7);
	print_char('\n');
	print_str("B_int16_t_azero2b7=");
	print_hex16(B_int16_t_azero2b7);
	print_char('\n');
	print_str("A_uint8_t_aone2b7=");
	print_hex16(A_uint8_t_aone2b7);
	print_char('\n');
	print_str("B_int16_t_aone2b7=");
	print_hex16(B_int16_t_aone2b7);
	print_char('\n');
	print_str("A_uint8_t_amax2b8=");
	print_hex16(A_uint8_t_amax2b8);
	print_char('\n');
	print_str("B_uint16_t_amax2b8=");
	print_hex16(B_uint16_t_amax2b8);
	print_char('\n');
	print_str("A_uint8_t_amin2b8=");
	print_hex16(A_uint8_t_amin2b8);
	print_char('\n');
	print_str("B_uint16_t_amin2b8=");
	print_hex16(B_uint16_t_amin2b8);
	print_char('\n');
	print_str("A_uint8_t_azero2b8=");
	print_hex16(A_uint8_t_azero2b8);
	print_char('\n');
	print_str("B_uint16_t_azero2b8=");
	print_hex16(B_uint16_t_azero2b8);
	print_char('\n');
	print_str("A_uint8_t_aone2b8=");
	print_hex16(A_uint8_t_aone2b8);
	print_char('\n');
	print_str("B_uint16_t_aone2b8=");
	print_hex16(B_uint16_t_aone2b8);
	print_char('\n');
	print_str("A_int16_t_amax2b9=");
	print_hex16(A_int16_t_amax2b9);
	print_char('\n');
	print_str("B_int8_t_amax2b9=");
	print_hex16(B_int8_t_amax2b9);
	print_char('\n');
	print_str("A_int16_t_amin2b9=");
	print_hex16(A_int16_t_amin2b9);
	print_char('\n');
	print_str("B_int8_t_amin2b9=");
	print_hex16(B_int8_t_amin2b9);
	print_char('\n');
	print_str("A_int16_t_azero2b9=");
	print_hex16(A_int16_t_azero2b9);
	print_char('\n');
	print_str("B_int8_t_azero2b9=");
	print_hex16(B_int8_t_azero2b9);
	print_char('\n');
	print_str("A_int16_t_aone2b9=");
	print_hex16(A_int16_t_aone2b9);
	print_char('\n');
	print_str("B_int8_t_aone2b9=");
	print_hex16(B_int8_t_aone2b9);
	print_char('\n');
	print_str("A_int16_t_aMinusOne2b9=");
	print_hex16(A_int16_t_aMinusOne2b9);
	print_char('\n');
	print_str("B_int8_t_aMinusOne2b9=");
	print_hex16(B_int8_t_aMinusOne2b9);
	print_char('\n');
	print_str("A_int16_t_amax2b10=");
	print_hex16(A_int16_t_amax2b10);
	print_char('\n');
	print_str("B_uint8_t_amax2b10=");
	print_hex16(B_uint8_t_amax2b10);
	print_char('\n');
	print_str("A_int16_t_amin2b10=");
	print_hex16(A_int16_t_amin2b10);
	print_char('\n');
	print_str("B_uint8_t_amin2b10=");
	print_hex16(B_uint8_t_amin2b10);
	print_char('\n');
	print_str("A_int16_t_azero2b10=");
	print_hex16(A_int16_t_azero2b10);
	print_char('\n');
	print_str("B_uint8_t_azero2b10=");
	print_hex16(B_uint8_t_azero2b10);
	print_char('\n');
	print_str("A_int16_t_aone2b10=");
	print_hex16(A_int16_t_aone2b10);
	print_char('\n');
	print_str("B_uint8_t_aone2b10=");
	print_hex16(B_uint8_t_aone2b10);
	print_char('\n');
	print_str("A_int16_t_aMinusOne2b10=");
	print_hex16(A_int16_t_aMinusOne2b10);
	print_char('\n');
	print_str("B_uint8_t_aMinusOne2b10=");
	print_hex16(B_uint8_t_aMinusOne2b10);
	print_char('\n');
	print_str("A_int16_t_amax2b11=");
	print_hex16(A_int16_t_amax2b11);
	print_char('\n');
	print_str("B_int16_t_amax2b11=");
	print_hex16(B_int16_t_amax2b11);
	print_char('\n');
	print_str("A_int16_t_amin2b11=");
	print_hex16(A_int16_t_amin2b11);
	print_char('\n');
	print_str("B_int16_t_amin2b11=");
	print_hex16(B_int16_t_amin2b11);
	print_char('\n');
	print_str("A_int16_t_azero2b11=");
	print_hex16(A_int16_t_azero2b11);
	print_char('\n');
	print_str("B_int16_t_azero2b11=");
	print_hex16(B_int16_t_azero2b11);
	print_char('\n');
	print_str("A_int16_t_aone2b11=");
	print_hex16(A_int16_t_aone2b11);
	print_char('\n');
	print_str("B_int16_t_aone2b11=");
	print_hex16(B_int16_t_aone2b11);
	print_char('\n');
	print_str("A_int16_t_aMinusOne2b11=");
	print_hex16(A_int16_t_aMinusOne2b11);
	print_char('\n');
	print_str("B_int16_t_aMinusOne2b11=");
	print_hex16(B_int16_t_aMinusOne2b11);
	print_char('\n');
	print_str("A_int16_t_amax2b12=");
	print_hex16(A_int16_t_amax2b12);
	print_char('\n');
	print_str("B_uint16_t_amax2b12=");
	print_hex16(B_uint16_t_amax2b12);
	print_char('\n');
	print_str("A_int16_t_amin2b12=");
	print_hex16(A_int16_t_amin2b12);
	print_char('\n');
	print_str("B_uint16_t_amin2b12=");
	print_hex16(B_uint16_t_amin2b12);
	print_char('\n');
	print_str("A_int16_t_azero2b12=");
	print_hex16(A_int16_t_azero2b12);
	print_char('\n');
	print_str("B_uint16_t_azero2b12=");
	print_hex16(B_uint16_t_azero2b12);
	print_char('\n');
	print_str("A_int16_t_aone2b12=");
	print_hex16(A_int16_t_aone2b12);
	print_char('\n');
	print_str("B_uint16_t_aone2b12=");
	print_hex16(B_uint16_t_aone2b12);
	print_char('\n');
	print_str("A_int16_t_aMinusOne2b12=");
	print_hex16(A_int16_t_aMinusOne2b12);
	print_char('\n');
	print_str("B_uint16_t_aMinusOne2b12=");
	print_hex16(B_uint16_t_aMinusOne2b12);
	print_char('\n');
	print_str("A_uint16_t_amax2b13=");
	print_hex16(A_uint16_t_amax2b13);
	print_char('\n');
	print_str("B_int8_t_amax2b13=");
	print_hex16(B_int8_t_amax2b13);
	print_char('\n');
	print_str("A_uint16_t_amin2b13=");
	print_hex16(A_uint16_t_amin2b13);
	print_char('\n');
	print_str("B_int8_t_amin2b13=");
	print_hex16(B_int8_t_amin2b13);
	print_char('\n');
	print_str("A_uint16_t_azero2b13=");
	print_hex16(A_uint16_t_azero2b13);
	print_char('\n');
	print_str("B_int8_t_azero2b13=");
	print_hex16(B_int8_t_azero2b13);
	print_char('\n');
	print_str("A_uint16_t_aone2b13=");
	print_hex16(A_uint16_t_aone2b13);
	print_char('\n');
	print_str("B_int8_t_aone2b13=");
	print_hex16(B_int8_t_aone2b13);
	print_char('\n');
	print_str("A_uint16_t_amax2b14=");
	print_hex16(A_uint16_t_amax2b14);
	print_char('\n');
	print_str("B_uint8_t_amax2b14=");
	print_hex16(B_uint8_t_amax2b14);
	print_char('\n');
	print_str("A_uint16_t_amin2b14=");
	print_hex16(A_uint16_t_amin2b14);
	print_char('\n');
	print_str("B_uint8_t_amin2b14=");
	print_hex16(B_uint8_t_amin2b14);
	print_char('\n');
	print_str("A_uint16_t_azero2b14=");
	print_hex16(A_uint16_t_azero2b14);
	print_char('\n');
	print_str("B_uint8_t_azero2b14=");
	print_hex16(B_uint8_t_azero2b14);
	print_char('\n');
	print_str("A_uint16_t_aone2b14=");
	print_hex16(A_uint16_t_aone2b14);
	print_char('\n');
	print_str("B_uint8_t_aone2b14=");
	print_hex16(B_uint8_t_aone2b14);
	print_char('\n');
	print_str("A_uint16_t_amax2b15=");
	print_hex16(A_uint16_t_amax2b15);
	print_char('\n');
	print_str("B_int16_t_amax2b15=");
	print_hex16(B_int16_t_amax2b15);
	print_char('\n');
	print_str("A_uint16_t_amin2b15=");
	print_hex16(A_uint16_t_amin2b15);
	print_char('\n');
	print_str("B_int16_t_amin2b15=");
	print_hex16(B_int16_t_amin2b15);
	print_char('\n');
	print_str("A_uint16_t_azero2b15=");
	print_hex16(A_uint16_t_azero2b15);
	print_char('\n');
	print_str("B_int16_t_azero2b15=");
	print_hex16(B_int16_t_azero2b15);
	print_char('\n');
	print_str("A_uint16_t_aone2b15=");
	print_hex16(A_uint16_t_aone2b15);
	print_char('\n');
	print_str("B_int16_t_aone2b15=");
	print_hex16(B_int16_t_aone2b15);
	print_char('\n');
	print_str("A_uint16_t_amax2b16=");
	print_hex16(A_uint16_t_amax2b16);
	print_char('\n');
	print_str("B_uint16_t_amax2b16=");
	print_hex16(B_uint16_t_amax2b16);
	print_char('\n');
	print_str("A_uint16_t_amin2b16=");
	print_hex16(A_uint16_t_amin2b16);
	print_char('\n');
	print_str("B_uint16_t_amin2b16=");
	print_hex16(B_uint16_t_amin2b16);
	print_char('\n');
	print_str("A_uint16_t_azero2b16=");
	print_hex16(A_uint16_t_azero2b16);
	print_char('\n');
	print_str("B_uint16_t_azero2b16=");
	print_hex16(B_uint16_t_azero2b16);
	print_char('\n');
	print_str("A_uint16_t_aone2b16=");
	print_hex16(A_uint16_t_aone2b16);
	print_char('\n');
	print_str("B_uint16_t_aone2b16=");
	print_hex16(B_uint16_t_aone2b16);
	print_char('\n');

	return 0;
}
