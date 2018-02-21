#include <stdio.h>

int main ()
{
	int des, src, i;
	for(i=0;i<50;i++)
	{
		srand(i);
		src = rand()%16;
		
		des = rand()%16;
		while(des == src)
		{
			des = rand()%16;
		}
		printf("src is %d, des is %d\n", src, des);
	}
}
