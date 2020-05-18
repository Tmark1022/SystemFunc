/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Mon 18 May 2020 02:03:04 PM CST
 @ File Name	: limit.c
 @ Description	: 
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

int main(int argc, char *argv[]) {
	printf("in %s : %s, short max = %d, short min = %d\n", __FILE__, __FUNCTION__, SHRT_MAX, SHRT_MIN);
	return 0;
}
