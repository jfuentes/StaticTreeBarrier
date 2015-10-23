/**
* Static Tree Barrier with atomic operations
* author: Joel Fuentes - joel.fuentes@uci.edu
**/

#include <stdio.h>
#include <threads.h>
#include "statictreebarrier.h"
#include "librace.h"
#include <stdlib.h>

#define NUMREADERS 3
#define RADIX 2

struct info{
    int value;
};



StaticTreeBarrier *barr;
int var = 0;

void testA(void * pointer){
	barr->await(* (int *)pointer);
	//store_32(&var, inf->value);
   printf("\nThread %d is done ", * (int *)pointer);
}




int user_main(int argc, char **argv){

	thrd_t B[NUMREADERS];

	int i=0;

	barr = new StaticTreeBarrier(NUMREADERS, RADIX);


	for (i = 0; i < NUMREADERS; i++){
		int*in = (int *) malloc(sizeof(int));
		*in=i;
		thrd_create(&B[i], &testA, in);
	}



	for (i = 0; i < NUMREADERS; i++)
		thrd_join(B[i]);


	return 0;
}
