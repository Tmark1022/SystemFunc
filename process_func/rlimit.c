// demo 获取和设置软硬限制
// sudo chown root rlimit ; sudo chmod u+s rlimit 设置用户ID， 使得用root权限执行代码

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
	setbuf(stdout, NULL);

	// first get
	struct rlimit limit;	
	if (-1 == getrlimit(RLIMIT_NOFILE, &limit)) {
		perror("getrlimit error");
	}	

	printf("RLIMIT_NOFILE : soft %ld, hard %ld , _SC_OPEN_MAX = %ld\n", limit.rlim_cur, limit.rlim_max, sysconf(_SC_OPEN_MAX)); 

	// get after set hard limit
	limit.rlim_max = 100000; 
	if (-1 == setrlimit(RLIMIT_NOFILE, &limit)) {
		perror("setrlimit error");
	}	
		
	if (-1 == getrlimit(RLIMIT_NOFILE, &limit)) {
		perror("getrlimit error");
	}	

	printf("after, RLIMIT_NOFILE : soft %ld, hard %ld , _SC_OPEN_MAX = %ld\n", limit.rlim_cur, limit.rlim_max, sysconf(_SC_OPEN_MAX)); 

	
	// get after set soft limit
	limit.rlim_cur = 100000; 
	if (-1 == setrlimit(RLIMIT_NOFILE, &limit)) {
		perror("setrlimit error");
	}	

	if (-1 == getrlimit(RLIMIT_NOFILE, &limit)) {
		perror("getrlimit error");
	}	

	printf("after, RLIMIT_NOFILE : soft %ld, hard %ld , _SC_OPEN_MAX = %ld\n", limit.rlim_cur, limit.rlim_max, sysconf(_SC_OPEN_MAX)); 
	
	if (argc != 2) 
		return 1;

	int filecnt = atoi(argv[1]);
	printf("try to open %d files after sleep one secend\n", filecnt);
	sleep(1);
	int fd;
	for(int i = 1;  i <= filecnt; ++i) {
		fd = open("aa.txt", O_RDONLY);
		if (-1 == fd) {
			perror("open error");
		} else {
			printf("now fd %d\n", fd);
		}	
	}		

	sleep(60);

	return 0;
}
