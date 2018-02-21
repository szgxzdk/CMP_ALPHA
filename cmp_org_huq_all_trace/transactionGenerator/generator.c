#include "header.h"
#include <time.h>

int generator(float probability)
{
	int seed;
	float prob;
	seed = rand()%100000;
	prob = (float)seed/(float)100000;
	if(prob <= probability)
		return 1;
	else
		return 0;	
}

int compare(int t, int i, int *r)
{
	int flag = 0, m;
	for(m=0;m<i;m++)
	{
		if(t == r[m])
		{
			flag = 1;
			break;
		}
	}
	return flag;
}
unsigned long long int sharers_generator(int src)
{
	int seed, sharer_num = 0, i;
	int sharers_record[15] = {0};
	unsigned long long int sharers;
	float prob;
	seed = rand()%10000;
	prob = (float)seed/(float)10000;
	sharers = 0;
	if(prob <= invalidation_one)
		sharer_num = 1;
	else if(prob > invalidation_one && prob <= invalidation_two)
		sharer_num = 2;
	else if(prob > invalidation_two && prob <= invalidation_three)
		sharer_num = 3;
	else if(prob > invalidation_three && prob <= invalidation_four)
		sharer_num = 4;
	else if(prob > invalidation_four && prob <= invalidation_five)
		sharer_num = 5;
	else if(prob > invalidation_five && prob <= invalidation_six)
		sharer_num = 6;
	else if(prob > invalidation_six && prob <= invalidation_seven)
		sharer_num = 7;
	else if(prob > invalidation_seven && prob <= invalidation_eight)
		sharer_num = 8;
	else if(prob > invalidation_eight && prob <= invalidation_nine)
		sharer_num = 9;
	else if(prob > invalidation_nine && prob <= invalidation_ten)
		sharer_num = 10;
	else if(prob > invalidation_ten && prob <= invalidation_eleven)
		sharer_num = 11;
	else if(prob > invalidation_eleven && prob <= invalidation_twelve)
		sharer_num = 12;
	else if(prob > invalidation_twelve && prob <= invalidation_thirteen)
		sharer_num = 13;
	else if(prob > invalidation_thirteen && prob <= invalidation_fourteen)
		sharer_num = 14;
	else if(prob > invalidation_fourteen && prob <= invalidation_fifteen)
		sharer_num = 15;
	for(i=0;i<sharer_num;i++)	
	{
		seed = rand()%total_num_node;
		while(compare(seed, i, sharers_record))
			seed = rand()%total_num_node;
		sharers_record[i] = seed;

	}
	for(i=0;i<sharer_num;i++)
		sharers = sharers | 1<<sharers_record[i];
	return sharers;
}


int src_gen()
{
	int src;
	srand(sim_cycle);
	src = rand()%total_num_node;
	return src;
}

int des_gen(int src)
{
	int des;
//	srand(sim_cycle);
	des = rand()%(total_num_node*10);
	while((des%10) == src)
	{
		des = rand()%(total_num_node*10);
	}
	return (des%10);
}
