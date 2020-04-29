/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Wed 29 Apr 2020 07:12:29 PM CST
 @ File Name	: daemon.c
 @ Description	: 简单创建daemon进程 
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
	
	if (0 != fork()) {
		exit(EXIT_SUCCESS);
	}

	if (-1 == setsid()) {
		perror("setsid error.");
		exit(EXIT_FAILURE);	
	}		

	// change working directory
	char * nowPath = getcwd(NULL, 0);
	if (-1 == chdir(nowPath)) {
		perror("chdir error");
		exit(EXIT_FAILURE);	
	}
		
	// change umask
	umask(022);	

	// close fd inherited from parent
	int openMax = sysconf(_SC_OPEN_MAX);	
	for(int i = 0; i < openMax; ++i) {
		close(i);
	}
	
	// do something
	while (1) {
		sleep(1);
	}

	return 0;
}
