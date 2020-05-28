/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Thu 28 May 2020 03:54:54 PM CST
 @ File Name	: test.c
 @ Description	: 
 ************************************************************************/
/*
 * /var/run 注意目录权限
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <daemon.h>

int main(int argc, char *argv[]) {
	
	char * cmd = strrchr(argv[0], '/');
	if (NULL == cmd) {
		cmd = argv[0];
	} else {
		++cmd;
	}

	Daemonize(cmd);
	SingletonDaemon(cmd);

	pause();

	return 0;
}
