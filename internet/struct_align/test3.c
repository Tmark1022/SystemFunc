/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Mon 11 May 2020 08:37:15 PM CST
 @ File Name	: test3.c
 @ Description	: 
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct A {
	char a;
	short b;
};

int main(int argc, char *argv[]) {
	printf("size is %ld\n", sizeof(struct A));
	return 0;
}
