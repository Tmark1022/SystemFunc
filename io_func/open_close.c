#include <stdio.h>
#include <stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<errno.h>

int main(int argc, char *argv[]){
	
	int fd = open("./hello.txt", O_WRONLY | O_CREAT | O_TRUNC | O_EXCL,  0644);	
	printf("fd = %d\n", fd);
	if (-1 == fd){
		perror("open file error:");
		exit(1);
	}
	else{
		close(fd);
	}


	return 0;
}
