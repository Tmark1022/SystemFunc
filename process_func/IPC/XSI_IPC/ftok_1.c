/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Sun 07 Jun 2020 12:30:20 PM CST
 @ File Name	: ftok_1.c
 @ Description	: 
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/ipc.h>

int main(int argc, char *argv[]) {
	if (3 != argc) {
		printf("usage : %s path prog_id\n", __FILE__);
		exit(EXIT_FAILURE);
	}	

	key_t res = ftok(argv[1], atoi(argv[2]));
	printf("res = %d\n", res);

	return 0;
}
