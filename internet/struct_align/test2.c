/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Mon 11 May 2020 05:32:03 PM CST
 @ File Name	: test2.c
 @ Description	: 
 ************************************************************************/
#pragma pack(4)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>

// 24
struct C {
	int i;
	long l;
	char c;		
};

// 最大类型long, 整个结构体size是8的整数倍
struct A{
	char a;			// 0
	short b;		// 2
	int c;			// 4
	long d;			// 8
	int e;			// 16
	short f;		// 20
	char g;			// 23
};

// 				default(8)	n = 4
struct B{
	char a;			// 0		0
	short b;		// 2		2
	char c[10];		// 3		3
	long d;			// 16		16
	struct C e;		// 24		24
	double f;		// 48		40	
	short g;		// 56		48
};
//			total	64		52
#define PRINT_STRUCT(x) do { printf(#x", size = %ld, c offset is %ld, i offset is %ld, l offset is %ld\n", \
		sizeof(x), offsetof(x, c), offsetof(x, i), offsetof(x, l));} while(0)

#define PRINT_STRUCT2(x) do { printf(#x", size = %ld, a offset is %ld, b offset is %ld, c offset is %ld, d offset is %ld, e offset is %ld, f offset is %ld, g offset is %ld\n", sizeof(x), offsetof(x, a), offsetof(x, b), offsetof(x, c), offsetof(x, d), offsetof(x, e), offsetof(x, f), offsetof(x, g));} while(0)

int main(int argc, char *argv[]) {
	printf("short = %ld, int = %ld, long = %ld, long long = %ld, double = %ld, float = %ld\n", sizeof(short), sizeof(int), sizeof(long), sizeof(long long), sizeof(double), sizeof(float));
	
	PRINT_STRUCT(struct C);
	PRINT_STRUCT2(struct A);
	PRINT_STRUCT2(struct B);
	return 0;
}

