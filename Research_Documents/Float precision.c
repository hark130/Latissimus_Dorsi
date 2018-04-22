#include <stdio.h>

int main(void)
{
	/*
	float num1 = 1.0;
	float num2 = 1.0;
	float result = 0;
	*/
	double num1 = 1.0;
	double num2 = 1.0;
	double result = 0;
	int counter = 0;

	
	while (num1 + num2 != num1)
	{
		++counter;
		num2 = num2 / 10.0;
	}
	printf("%2d digits accuracy in calculations\n", counter);

	num2 = 1.0;
	counter = 0;

	while (1)
	{
		result = num1 + num2;
		if (result == num1)
		{
			break;
		}
		++counter;
		num2 = num2 / 10.0;
	}
	printf("%2d digits accuracy in storage\n", counter);

	getchar();
	return 0;
}
