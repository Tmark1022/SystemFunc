/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Tue 09 Jun 2020 02:08:01 PM CST
 @ File Name	: test1.c
 @ Description	: 
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	char *a = "abc";
	const char *b = "abc";
	char const *c = "abc";
	const char * const d  = "abc";
	const char e[]  = "abc";
	char f[]  = "abc";
	char g[]  = "abc";

	printf("%x, %x, %x, %x, %x, %x, %x\n", a, b, c, d, e, f, g);

	return 0;
}
