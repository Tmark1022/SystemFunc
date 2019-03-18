#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>



int main(int argc, char *argv[]){
	int fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC);
	int dup_fd = dup2(fd, 7);

	int fcntl_fd = fcntl(fd, F_DUPFD, 7);


	printf("fd = %d, dup_fd = %d, fcntl_fd = %d\n", fd, dup_fd, fcntl_fd);
	
	write(fd, "12345", 5);
	close(fd);
	write(dup_fd, "67890", 5);
	close(dup_fd);
	write(fcntl_fd, "abcdef\n", 7);
	close(fcntl_fd);

	return 0;
}
