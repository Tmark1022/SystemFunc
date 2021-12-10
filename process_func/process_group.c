/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Thu 30 Apr 2020 09:53:55 AM CST
 @ File Name	: process_group.c
 @ Description	: apue说控制终端键入的信号会发送给前台进程的所有进程，但是结果怎么设置捕捉后只被一个进程调用？？？【环境：ubuntu 18.04， 内核: Linux 4.15.0-96-generic】 , 调用方法， shell 执行 ./process_group| ./process_group | ./process_group 

ans: 20211210, 的确会发送到进程组内的所有进程中， 之前以为捕捉到一个问题出现在写标准输出, 因为用管道启动三个进程，有两个进程的标准输出并不是控制终端了;
		如果写文件或stderr， 就能看到输出了。

 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

void handler(int signum) 
{
	char path[1024];	
	sprintf(path, "./%d.log", getpid());
	int fd = open(path, O_RDWR | O_CREAT | O_APPEND, 0664);

	// 不知为什么只被一个进程捕捉
	char buf[1024];	
	int tpgid = tcgetpgrp(STDIN_FILENO);	
	char errstr[256] = "";
	int no = 0;
	if (-1 == tpgid) {
		no = errno;
		strcpy(errstr, strerror(errno));
	} 
	sprintf(buf, "fd :%d, ppid : %d, pid : %d, pgrpid : %d, sid : %d, tpgid : %d, errno : %d, errstr : %s\n", fd, getppid(), getpid(), getpgid(0), getsid(0), tpgid, no, errstr);
	// write(STDOUT_FILENO, buf, strlen(buf));
	write(STDERR_FILENO, buf, strlen(buf));
	write(fd, buf, strlen(buf));
	
	close(fd);
}	

int main(int argc, char *argv[]) {
	struct sigaction new;	
	new.sa_flags = 0;
	sigemptyset(&new.sa_mask);	
	new.sa_handler = handler;
	sigaction(SIGQUIT, &new, NULL);

	while(1)
		pause();
		
	return 0;
}

