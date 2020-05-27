/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Wed 27 May 2020 04:36:47 PM CST
 @ File Name	: my_syslog.c
 @ Description	: syslog 相关使用
 ************************************************************************/
#include <asm-generic/errno-base.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syslog.h>
#include <unistd.h>
#include <syslog.h>
#include <errno.h>

int main(int argc, char *argv[]) {
	if (argc <= 1) {
		errno = EINVAL;
		printf("usage : my_syslog logmsg1 logmsg2 ...\n");
		syslog(LOG_ERR, "%s : %m", argv[0]);
		return 0;
	}
	openlog(argv[0], LOG_PERROR | LOG_PID, LOG_USER);

	syslog(LOG_INFO, "hello world, i am tmark\n");
	for (int i = 1; i < argc; ++i) {
		syslog(LOG_ERR, "%s", argv[i]);
	}	

	errno = ESPIPE;
	syslog(LOG_ERR, "error, %m");

	closelog();
	return 0;
}
