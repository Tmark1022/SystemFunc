#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

char buff[1024];


int main(int argc, char *argv[]) {
	int fd = open(argv[1], O_RDONLY);
	if (-1 == fd) {
		perror("open error");
		exit(EXIT_FAILURE);
	}

	int cnt = read(fd, buff, 1024);
	printf("read cnt %d", cnt);
	if (cnt > 0) {
		write(STDOUT_FILENO, buff, cnt);
	}

	return 0;
}
