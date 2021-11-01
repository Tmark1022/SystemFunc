/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Mon 01 Nov 2021 05:44:36 PM CST
 @ File Name	: main.c
 @ Description	: 
 ************************************************************************/
#include <stdio.h>
#include <malloc.h>

int main(int argc, char *argv[]) {
	void * p = malloc(32);
	free(p);
	return 0;
}
