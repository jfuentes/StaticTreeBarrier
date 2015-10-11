#include <stdio.h>
#include <threads.h>

#include "statictreebarrier.h"

#include "librace.h"

#define NUMREADERS 7
#define RADIX 2

struct info
{
    int value;
};



StaticTreeBarrier *barr;
int var = 0;

void threadA(void * pointer)
{
	info *inf = (info*) pointer;
	barr->await(inf->value);
	var++;

}




int user_main(int argc, char **argv)
{
	printf("Hi there!");
	thrd_t B[NUMREADERS];

	int i;

	barr = new StaticTreeBarrier(NUMREADERS, RADIX);


	for (i = 0; i < NUMREADERS; i++){
		info in;
		in.value=i;
		thrd_create(&B[i], &threadA, &in);
	}
	for (i = 0; i < NUMREADERS; i++)
		thrd_join(B[i]);


	return 0;
}
