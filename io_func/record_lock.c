/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Thu 28 May 2020 11:38:21 AM CST
 @ File Name	: record_lock.c
 @ Description	: 文件记录锁(process-associate), 进程级别的，常用于daemon进程实现单例。
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

// 对fd指向的整个文件region加写锁，同时只能有一个进程加成功
int lockfile(int fd)
{
	struct flock lc;	
	lc.l_type = F_WRLCK;
	lc.l_whence = SEEK_SET;
	lc.l_start = 0;
	lc.l_len = 0;
		
	return fcntl(fd, F_SETLK, &lc);
}

int main(int argc, char *argv[]) {

	int fd = open("lockfile", O_WRONLY | O_CREAT, 0644);
	if (fd == -1) {
		perror("open file error");
		exit(EXIT_FAILURE);
	}

	if (0 != lockfile(fd)) {
		printf("lock file failed\n");
		exit(EXIT_FAILURE);
	} else {
		printf("lock file successfully, now pause\n");
	}
	
	pause();
		
	return 0;
}

