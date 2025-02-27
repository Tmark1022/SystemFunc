/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Thu 27 Feb 2025 03:21:34 PM CST
 @ File Name	: nonlocal-gotos.c
 @ Description	: setjmp && longjmp demo
 ************************************************************************/
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>


jmp_buf loc;
int totalLoop = 0;

void recursion(int lv)
{
	++totalLoop;
	if (rand() % 51 == 50 || lv >= 100) {
		longjmp(loc, lv);
	}
	recursion(lv + 1);	
}

int main(int argc, char *argv[]) {

	srand(time(NULL));
	int res = setjmp(loc); 
	if (res) {
		printf("total loop cnt : %d,level %d\n", totalLoop, res);
		sleep(5);
	}
	recursion(1);	

	return 0;
}
