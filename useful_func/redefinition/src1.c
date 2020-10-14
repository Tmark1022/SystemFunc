/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Wed 14 Oct 2020 09:40:55 AM CST
 @ File Name	: src1.c
 @ Description	: 
 ************************************************************************/
#include "header.h"

void foo();


// 在bss 段中， 链接阶段并不会redifinition
int bss;
int bss;

int main(int argc, char *argv[]) {

	foo();
	arr[1] = 'i';  
	printf("arr is %s\n", arr);

	bss = 100;
	printf("bss is %d\n", bss);

	return 0;
}
