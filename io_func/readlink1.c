#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SIZE 1024
char buf[SIZE];

int main(int argc, char *argv[]){
	int n = readlink(argv[1], buf, SIZE);
	if (-1 == n){
		perror("readlink error");
		exit(1);
	}

	printf("string size is %d\n, string = %s\n", n, buf);

	

	return 0;
}
