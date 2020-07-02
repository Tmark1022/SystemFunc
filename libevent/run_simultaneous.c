/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Thu 02 Jul 2020 11:04:41 AM CST
 @ File Name	: run_simultaneous.c
 @ Description	: 开两个进程同时跑
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	

	// 本机通讯基本没有延迟， 很难模拟同时发送SYN
	int pid = fork();	
	if (-1 == pid) {
		perror("fork error");	
		exit(1);
	} else if (pid == 0) {
		// child
		execlp("sh", "sh", "-c", "./libevent_stream_client -h 192.168.232.129 -p 8888 -b 9999 -s 192.168.232.129", NULL);
	} else {
		// parent
		execlp("sh", "sh", "-c", "./libevent_stream_client -h 192.168.232.129 -p 9999 -b 8888 -s 192.168.232.129", NULL);	
	}


	return 0;
}
