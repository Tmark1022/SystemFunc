#include <stdio.h>
#include <stdlib.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>
#include<errno.h>

char *buf[1024];
char * TryAgain = "try again\n";

int main(int argc, char *argv[]){
	int flags = fcntl(STDIN_FILENO, F_GETFL);
	printf("flags = %d\n", flags);

	flags |= O_NONBLOCK;
	if (fcntl(STDIN_FILENO, F_SETFL, flags) == -1){
		perror("fcntl error");
		exit(1);
	}

	int n;
	while((n = read(STDIN_FILENO, buf, 1024)) != 0){
		
		if (-1 == n){
			if (EAGAIN == errno){
				write(STDOUT_FILENO, TryAgain, strlen(TryAgain));
				sleep(4);
			}
			else{
				perror("read error");
				exit(1);
			}
		}
		else{
			write(STDOUT_FILENO, buf, n);
		}
	}



	return 0;
}
