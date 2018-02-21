#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
	int t;
	int i = 0;
	double result = 1.0;
	float a = 0, b = 0;
//	srand(time(NULL));
	{
		t = rand()%(10);
		//printf("t is %d, range of %d\n", t, 10*(i+1));
		if(t < 1)
			a ++;
		else 
			b ++;
	}
	float sum = a + b;
	for(i=0; i<3; i++)
		result = result*2;
		
	printf("result is %d\n", (int)result);
}
