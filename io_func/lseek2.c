#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include<string.h>

#define SIZE 1024
char * buf[SIZE];



int main(int argc, char *argv[]){
	
	int fd = open(argv[1], O_RDONLY);
	if (-1 == fd){
		perror("open file error");
		exit(1);
	}

	int length = lseek(fd, 0, SEEK_END);
	printf("file %s 's size is %d\n", argv[1], length);

	close(fd);

	return 0;
}
