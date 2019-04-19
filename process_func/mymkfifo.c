#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char *argv[]){
	if (-1 == mkfifo(argv[1], 0774)){
		perror("mkfifo error");
		exit(1);
	}
	

	return 0;
}
