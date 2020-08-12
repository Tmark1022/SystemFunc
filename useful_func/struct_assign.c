/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Wed 12 Aug 2020 09:57:10 AM CST
 @ File Name	: struct_assign.c
 @ Description	: 结构体赋值
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct test_t {
	char a;
	int b;
	double c[2];
	char * d;
}aaa = {'v', 10, {11.0,22.0}, "hello world"}, bbb;

void assign()
{
	bbb = aaa;
}

int main(int argc, char *argv[]) {
	assign();
	printf("%c, %d, %f, %f, %s\n", bbb.a, bbb.b, bbb.c[0], bbb.c[1], bbb.d);
	return 0;
}
