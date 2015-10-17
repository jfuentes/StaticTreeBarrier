/**
* Static Tree Barrier with atomic operations
* author: Joel Fuentes - joel.fuentes@uci.edu
**/

#include <stdio.h>
#include <threads.h>
#include "statictreebarrier.h"
#include "librace.h"

#define NUMREADERS 7
#define RADIX 2

struct info{
    int value;
};



StaticTreeBarrier *barr;
int var = 0;

void testA(void * pointer){
	info *inf = (info*) pointer;
	printf("\nThread %d has arrived ", inf->value);
	barr->await(inf->value);
	store_32(&var, inf->value);
   printf("\nThread %d is done ", inf->value);
}




int user_main(int argc, char **argv){

	thrd_t B[NUMREADERS];

	int i;

	barr = new StaticTreeBarrier(NUMREADERS, RADIX);


	for (i = 0; i < NUMREADERS; i++){
		info in;
		in.value=i;
		thrd_create(&B[i], &testA, &in);
	}
	for (i = 0; i < NUMREADERS; i++)
		thrd_join(B[i]);


	return 0;
}
