#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFSIZE 1024
char buff[BUFFSIZE];

int main(int argc, char *argv[]) {
	getlogin_r(buff, BUFFSIZE);
	printf("login name : %s\n", buff);

	return 0;
}
