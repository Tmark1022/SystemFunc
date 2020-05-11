/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Mon 11 May 2020 04:01:15 PM CST
 @ File Name	: test.c
 @ Description	: 
 ************************************************************************/
// set packing size, suppress padding
#pragma pack(1)


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

#define PRINT_STRUCT(x) do { printf(#x", size = %ld, c offset is %ld, i offset is %ld, l offset is %ld\n", \
		sizeof(x), offsetof(x, c), offsetof(x, i), offsetof(x, l));} while(0)


int main(int argc, char *argv[]) {
	printf("short = %ld, int = %ld, long = %ld, long long = %ld, double = %ld, float = %ld\n", sizeof(short), sizeof(int), sizeof(long), sizeof(long long), sizeof(double), sizeof(float));
	
	PRINT_STRUCT(struct A);
	PRINT_STRUCT(struct B);
	PRINT_STRUCT(struct C);
	PRINT_STRUCT(struct D);
	return 0;
}
