#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
	
	int fd = open(argv[1], O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
	if (-1 == fd) {
		perror("open file failed");
		exit(1);
	}
	
	if (-1 == dup2(fd, STDOUT_FILENO)) {
		perror("dup2 error");
		exit(1);
	}	

	close(fd);
	printf("hello world\n");
	char buff[1024];	
	while (EOF != scanf("%s", buff)) {
		write(STDOUT_FILENO, buff, strlen(buff));
	}
	
	close(STDOUT_FILENO);
	return 0;
}
