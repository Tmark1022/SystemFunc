#include <stdio.h>
#include <stdlib.h>
#include<sys/stat.h>
#include<unistd.h>
#include<string.h>
#include<fcntl.h>

char * buf[1024];
#define Size 1024

int main(int argc, char *argv[]){
	int fd1 = open(argv[1], O_RDONLY);
	if (fd1 == -1){
		perror("open file error:");
		exit(1);
	}

	printf("fd1 = %d\n", fd1);
	
	int flags1 = fcntl(fd1, F_GETFL);
	if (-1 == flags1){
		perror("fcntl error");
		exit(1);
	}

	printf("flags1 = %d\n", flags1);

	int fd2 = open(argv[1], O_RDONLY | O_NONBLOCK);
	if (fd2 == -1){
		perror("open file error:");
		exit(1);
	}
	
	printf("fd2 = %d\n", fd2);
   
	int flags2 = fcntl(fd2, F_GETFL);
	if (-1 == flags2){
		perror("fcntl error");
		exit(1);
	}

	printf("flags2 = %d\n", flags2);

	int fd3 = open(argv[1], O_WRONLY);
	if (fd3 == -1){
		perror("open file error:");
		exit(1);
	}
	
	printf("fd3 = %d\n", fd3);
   
	int flags3 = fcntl(fd3, F_GETFL);
	if (-1 == flags3){
		perror("fcntl error");
		exit(1);
	}

	printf("flags3 = %d\n", flags3);

	int fd4 = open(argv[1], O_RDWR);
	if (fd4 == -1){
		perror("open file error:");
		exit(1);
	}
	
	printf("fd4 = %d\n", fd4);
   
	int flags4 = fcntl(fd4, F_GETFL);
	if (-1 == flags4){
		perror("fcntl error");
		exit(1);
	}

	printf("flags4 = %d\n", flags4);




	return 0;
}
