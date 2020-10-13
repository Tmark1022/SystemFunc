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
#include <stddef.h>

struct test_t {
	char a;
	int b;
	double c[2];
	char * d;
}aaa = {'v', 10, {11.0,22.0}, "hello world"}, bbb;

struct AA{
	unsigned short a:4;			// struct bit field
	unsigned short b:2;
	unsigned char c;			// short 还有10bit， 放得下一个char
};

void assign()
{
	bbb = aaa;
}

int main(int argc, char *argv[]) {
	assign();
	printf("%c, %d, %f, %f, %s\n", bbb.a, bbb.b, bbb.c[0], bbb.c[1], bbb.d);


	struct AA a;
	a.a = 7;
	a.b = 1;
	a.c = 1;
	printf("sizeof AA is %ld, offset c %ld\n", sizeof(a), offsetof(struct AA, c));
	short * ptr = (short *)&a;
	printf("*ptr is %d, Ox%x\n", *ptr, *ptr);
	
	return 0;
}
