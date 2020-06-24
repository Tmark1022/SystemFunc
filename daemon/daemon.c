/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Thu 28 May 2020 02:25:31 PM CST
 @ File Name	: daemon.c
 @ Description	: 
 ************************************************************************/
#include "daemon.h"
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <unistd.h>

void PrintError(FILE * stream, int my_errno, const char * headStr, int exitCode)
{
	if (!my_errno) {
		my_errno = errno;
	}	
	fprintf(stream, "%s : %s\n", headStr, strerror(my_errno));	
	if (exitCode) {
		exit(exitCode);	
	}
}

void Daemonize(const char * cmd, int facility)
{
	// 1. clear umask mode
	umask(0);

	// 2. first fork, 保证不是进程组长
	int pid = fork();
	if (-1 == pid) {
		PrintError(stderr, 0, "first fork failed", EXIT_FAILURE);
	} else if (pid != 0) {
		exit(EXIT_SUCCESS);	
	}	

	// 3. 创建会话， 成为进程组组长， 丢弃控制终端 
	int pgid = setsid();
	if (-1 == pgid) {
		PrintError(stderr, 0, "setsid failed", EXIT_FAILURE);
	}

	// 再次fork， 使进程不是进程组长， 避免未来分配到控制终端 （还有忽略SIGHUP， 挂起终端会向控制进程发送SIGHUP）	
	struct sigaction sa;
	sa.sa_flags = 0;
	sa.sa_handler = SIG_IGN;
	if (-1 == sigemptyset(&sa.sa_mask)) {
		PrintError(stderr, 0, "sigemptyset failed", EXIT_FAILURE);
	}
	if (-1 == sigaction(SIGHUP, &sa, NULL)) {
		PrintError(stderr, 0, "sigaction failed", EXIT_FAILURE);
	}	
	pid = fork();
	if (-1 == pid) {
		PrintError(stderr, 0, "second fork failed", EXIT_FAILURE);
	} else if (pid != 0) {
		exit(EXIT_SUCCESS);	
	}	

	// 4. 改变当前工作目录
	if (-1 == chdir("/")) {
		PrintError(stderr, 0, "chdir failed", EXIT_FAILURE);
	}
	
	// 5. 关闭所有打开的文件描述符, 这步后标准输出等已经关闭， 往后的程序记录log 请使用syslog
	struct rlimit rlim;
	if (-1 == getrlimit(RLIMIT_NOFILE, &rlim)) {
		PrintError(stderr, 0, "getrlimit failed", EXIT_FAILURE);
	}	
	int maxfd;
	if (RLIM_INFINITY == rlim.rlim_max) {
		maxfd = 1024;
	} else {
		maxfd = rlim.rlim_max;	
	}
	for (int fd = 0; fd < maxfd; ++fd) {
		close(fd);
	}

	// 6. 重定向0,1,2 /dev/null 使调用标准io失效
	openlog(cmd, LOG_PID, facility);
	int fd0 = open("/dev/null", O_RDONLY);
	int fd1 = dup(fd0);
	int fd2 = dup(fd0);
	if (0 != fd0 || 1 != fd1 || 2 != fd2) {
		syslog(LOG_ERR,"unexpected file descripters, %d,%d,%d", fd0, fd1, fd2);
		exit(EXIT_FAILURE);
	}

	// other
	// ...

	syslog(LOG_NOTICE,"programe daemonize successfully");
}

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

void SingletonDaemon(const char * cmd)
{
	char path[256];		
	sprintf(path, "%s%s.pid", DAEMON_PID_PATH, cmd);
	printf("%s\n", path);
	int fd = open(path, O_WRONLY | O_CREAT, 0644);
	if (-1 == fd) {
		syslog(LOG_ERR, "SingletonDaemon, open failed, %m");
		exit(EXIT_FAILURE);	
	}
	
	int ret = lockfile(fd);
	if (0 != ret) {
		syslog(LOG_ERR, "someone already start");
		exit(EXIT_FAILURE);	
	} else {	
		syslog(LOG_NOTICE, "lock file successfuly, %d", ret);
	}
	
	ftruncate(fd, 0);	
	char tmp[100];
	sprintf(tmp, "%d", getpid());
	int cnt = strlen(tmp);
	if (cnt != write(fd, tmp, cnt)) {
		syslog(LOG_ERR, "write pid file error, %m");
		exit(EXIT_FAILURE);	
	}
	
}


