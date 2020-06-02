/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Tue 02 Jun 2020 04:19:16 PM CST
 @ File Name	: fifo_w.c
 @ Description	: 
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc, char *argv[]) {
	int fd = open("./fifo_tmp", O_WRONLY); 	
	if (-1 == fd) {
		perror("open failed");
		exit(1);
	}

	char buf[1024];	
	while (NULL != fgets(buf, 1024, stdin)) {
		write(fd, buf, strlen(buf));
	}

	close(fd);

	return 0;
}
