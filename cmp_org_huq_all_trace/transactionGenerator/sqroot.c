#include <math.h>
#include "header.h"

int sqrt(int num)
{
	double result;
	result = sqrt((double)num);
	return result;
}


 int main()
{
	int a;
	a = sqrt(16);
	printf("a is %d\n", a);
	return 0;
} 
