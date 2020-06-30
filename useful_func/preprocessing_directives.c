/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Tue 30 Jun 2020 10:08:28 AM CST
 @ File Name	: preprocessing_directives.c
 @ Description	: 预处理命令测试
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MARCO_TEST_ERROR

#if !defined(MARCO_TEST_ERROR)
#error "miss marco MARCO_TEST_ERROR\n"
#endif


int main(int argc, char *argv[]) {
	printf("hello world\n");
	return 0;
}
