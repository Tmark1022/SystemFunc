#include <stdio.h>
#include <stdlib.h>
#include<sys/stat.h>
#include<unistd.h>
#include<string.h>
#include<fcntl.h>

char * buf[1024];
#define Size 1024

int main(int argc, char *argv[]){
	int fd1 = open("/dev/tty", O_RDONLY);
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

	printf("flags1 = %d, O_RDONLY = %d, O_NONBLOCK = %d\n", flags1, O_RDONLY, O_NONBLOCK);
	if (flags1 & O_NONBLOCK){
		printf("is onoblick");
	}

/*	
	int n;
	while((n = read(fd1, buf, Size)) != 0){
		if (-1 == n){
			perror("read error:");
			exit(1);
		}
		
		printf("n = %d\n", n);
			
		write(STDOUT_FILENO, buf, n);
	}
*/

	int fd2 = open("/dev/tty", O_RDONLY | O_NONBLOCK);
	if (fd2 == -1){
		perror("open file error:");
		exit(1);
	}
	
	printf("fd2 = %d\n", fd1);
   
	int flags2 = fcntl(fd2, F_GETFL);
	if (-1 == flags2){
		perror("fcntl error");
		exit(1);
	}

	printf("flags2 = %d, O_RDONLY = %d, O_NONBLOCK = %d\n", flags2, O_RDONLY, O_NONBLOCK);
	if (flags2 & O_NONBLOCK){
		printf("is onoblick");
	}



	



	return 0;
}
