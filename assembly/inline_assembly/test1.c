/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Mon 23 May 2022 02:37:12 PM CST
 @ File Name	: test1.c
 @ Description	: 
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int func(int a, int b)
{
	__asm__(
		"movl %0, %%eax\n\t"
		"movl %1, %%ebx\n\t"
		"movl %%eax, %1\n\t"
		"movl %%ebx, %0\n\t"
		:"=m"(a), "=m"(b)         /* output */
		:"m"(a), "m"(b)        /* input */
		:"%eax", "%ebx", "memory"         /* clobbered register */
	);
	return a - b;
}

int main(int argc, char *argv[]) {
    int a = 10, b = 200, c;
	c = func(a, b);
	printf("Result: %d, %d, %d\n", a, b, c);
	return 0;

}
