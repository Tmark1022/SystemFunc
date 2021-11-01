/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Thu 28 Oct 2021 03:38:42 PM CST
 @ File Name	: main2.c
 @ Description	: 
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int lala = 1000;
int lala_bss;
extern int val1;
extern int val2;


int main(int argc, char *argv[]) {
	printf("lala %lx, lala_bss %lx, val1 %lx, val2 %lx\n", (long)&lala, (long)&lala_bss, (long)&val1, (long)&val2);
	printf("val1 %d, val2 %d\n", val1, val2);
	return 0;
}
