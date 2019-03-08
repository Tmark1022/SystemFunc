#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>
#include<string.h>

char * buf[1024];
#define SIZE 1024

int main(int argc, char *argv[]){
	
	int fd1 = open("/dev/tty", O_RDONLY | O_NONBLOCK);
	if (fd1 == -1){
		perror("open file error:");
		exit(1);
	}

	int n;
	int sleepcnt = 0;
	while((n = read(fd1, buf, SIZE)) != 0){
		if(-1 == n){
			// 非阻塞或者读错误
			if (errno == EAGAIN){
				sleepcnt += 1;
				if (sleepcnt > 5){
					write(STDOUT_FILENO, "too many times\n", strlen("too many times\n"));
					break;
				}
				// 非阻塞
				write(STDOUT_FILENO, "try again.\n", strlen("try again.\n"));
				sleep(5);
			}
			else{
				// 读错误
				perror("read error:");
				exit(1);
			}


		}
		else{
			sleepcnt = 0;
			if (write(STDOUT_FILENO, buf, n) == -1){
				perror("write error:");
				exit(1);
			}
			sleep(5);
		}



	}


	return 0;
}
