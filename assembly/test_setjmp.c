/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Fri 26 Nov 2021 05:10:51 PM CST
 @ File Name	: test_setjmp.c
 @ Description	: 
 ************************************************************************/
#include <stdio.h>
#include <setjmp.h>

jmp_buf b;

void f() 
{
	longjmp(b, 1);
}

int main(int argc, char *argv[]) {
	if (setjmp(b)) {
		printf("World!");
	} else {
		printf("Hello ");
		f();
	}
	return 0;
}
