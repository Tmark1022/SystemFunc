/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Thu 27 Aug 2020 10:20:46 AM CST
 @ File Name	: unix_domain_test.c
 @ Description	: 
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stddef.h>

struct AA{
	char a;
	int b;
	double c;
};

#define myoffset(a, b) (unsigned long)(&((a *)0)->b)

int main(int argc, char *argv[]) {

	printf("offset %ld\n", myoffset(struct AA, c));
	
	// 生成临时文件, 目录在P_tmpdir 中
	char buf[1024];
	printf("tmp name is %s\n", tmpnam(buf));
	return 0;
}
