#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(int argc, char *argv[]){
	
	int fd = open(argv[1], O_RDWR | O_APPEND | O_CREAT, 0664);
	if(-1 == fd){
		perror("open error");
		exit(1);
	}
	
	int new_fd = dup(fd);
	if (new_fd == -1){
		perror("dup error");
		exit(1);
	}

	int flags1 = fcntl(fd, F_GETFL);
	int flags2 = fcntl(new_fd, F_GETFL);
	printf("fd(%d) flags = %d, new_fd(%d) flags = %d\n", fd, flags1, new_fd, flags2);

	flags1 |= O_NONBLOCK;
	fcntl(fd, F_SETFL, flags1);
	
	flags1 = fcntl(fd, F_GETFL);
	flags2 = fcntl(new_fd, F_GETFL);
	printf("fd(%d) flags = %d, new_fd(%d) flags = %d\n", fd, flags1, new_fd, flags2);

	
	write(fd, "12345", 5);
	write(new_fd, "67890", 5);

	return 0;
}
