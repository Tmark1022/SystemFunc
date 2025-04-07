/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Thu 04 Jun 2020 04:19:10 PM CST
 @ File Name	: popen_1.c
 @ Description	: 标准库函数popen创建管道并通信 (子进程从标准输入获得数据)
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/cdefs.h>
#include <unistd.h>

// popen 只能构建一条unidirectional的管道，不能用于创建两条不同向向的管道（调用两次popen fork两个进程， 而不能在一个fork进程中绑定两条管道） 

int main(int argc, char *argv[]) {
	
	// 使用tr进行大小写转换	
	FILE *fr = popen("tr a-z A-Z", "r");
	if (NULL == fr) {
		perror("popen error");
		exit(1);
	}

	char buf[1024];

	while (NULL != fgets(buf, 1024, fr)) {
		printf("%s", buf);
	}

	printf("eof %d, error %d", feof(fr), ferror(fr));
	int ret = pclose(fr);
	if (-1 == ret) {
		perror("pclose error");
		exit(1);	
	}	
	printf("return status is %d\n", ret);

	return 0;
}
