/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Mon 11 May 2020 04:01:15 PM CST
 @ File Name	: test.c
 @ Description	: 
 ************************************************************************/
// set packing size, suppress padding
// pragma pack(n), individual member 的size小于n还是按照自身的size大小进行内存对齐，大于n的就按照n进行。
// Expected #pragma pack parameter to be '1', '2', '4', '8', or '16' on gcc compiler
// 64位架构， 相当于默认值n为8
// 32位架构， 相当于默认值n为4

// #pragma pack(4)



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>

// 16
struct A {
	char c;		
	int i;	
	long l;
};

struct B {
	int i;
	char c;		
	long l;
};

struct C {
	int i;
	long l;
	char c;		
};

struct D {
	long l;
	int i;
	char c;		
};

struct F {
	char c;		
	long l;
	int i;
};

struct G{
	char c;
	struct F l;
	int i;
};

#define PRINT_STRUCT(x) do { printf(#x", size = %ld, c offset is %ld, i offset is %ld, l offset is %ld\n", \
		sizeof(x), offsetof(x, c), offsetof(x, i), offsetof(x, l));} while(0)


int main(int argc, char *argv[]) {
	void * aa;
	printf("short = %ld, int = %ld, long = %ld, long long = %ld, double = %ld, float = %ld, void * = %ld\n", sizeof(short), sizeof(int), sizeof(long), sizeof(long long), sizeof(double), sizeof(float), sizeof(aa));
	
	PRINT_STRUCT(struct A);
	PRINT_STRUCT(struct B);
	PRINT_STRUCT(struct C);
	PRINT_STRUCT(struct D);
	PRINT_STRUCT(struct F);
	PRINT_STRUCT(struct G);
	return 0;
}
