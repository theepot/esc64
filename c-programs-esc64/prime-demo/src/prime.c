extern void show_in_r0(int number);

int is_prime(int number)
{
	int divisor, remainder;
	for(divisor = 3; divisor < number; divisor += 2)
	{
		remainder = number % divisor;
		if(remainder == 0)
		{
			return 0;
		}
	}
	return 1;
}

int main(int argc, char** argv)
{
	int number;
	for(number = 7; number < 0xFFF0; number += 2)
	{
		if(is_prime(number))
		{
			show_in_r0(number);
		}
	}
	return 0;
}


