// 匿名内存映射 (只能在有血缘的进程间进行进程间的通信)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>

typedef struct {
	int id;
	char name[64];
}Student;

int main(int argc, char *argv[]) {
	Student a = {1, "tmark"};  
	Student * point = mmap(NULL, 1024, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);		// shared
	memcpy(point, &a, sizeof(a));	

	int pid = fork();
	if (-1 == pid) {
		perror("fork error");
		exit(EXIT_FAILURE);
	} else if (0 == pid) {
		// child
		int idx = 1;
		while(idx <= 20){
			printf("id : %d, name : %s\n", point->id, point->name);
			sleep(1);
			++idx;
		}			
	} else {
		// parent
		for(int i = 1; i <= 10; ++i) {
			sleep(1);
			point->id += 1;	
		}	
		wait(NULL);
		printf("wait end\n");
	}

	return 0;
}
