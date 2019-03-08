#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include<string.h>

#define SIZE 1024
char * buf[SIZE];

int main(int argc, char *argv[]){
	int fd = open(argv[1], O_RDWR);
	if (-1 == fd){
		perror("open file error");
		exit(1);
	}

	int length = lseek(fd, 83, SEEK_END);
	
	printf("file %s 's size is %d\n", argv[1], length);
	

	int res = write(fd, "\0", 1);
	if (-1 == res){
		perror("write error");
		exit(1);
	}

	close(fd);

	return 0;
}
