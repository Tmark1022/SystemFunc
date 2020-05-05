/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Tue 05 May 2020 02:34:52 PM CST
 @ File Name	: sb.c
 @ Description	: 
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	int i = 10;
//	while (i-- > 0)
	while (i --> 0) {
		printf("%d\n", i);
		fflush(NULL);
	}
	return 0;
}
