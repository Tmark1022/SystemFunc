/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Sun 10 May 2020 05:15:24 PM CST
 @ File Name	: byteorder.c
 @ Description	: 字节序判别函数
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>


typedef union Elem{
	uint32_t no;
	uint8_t arr[4];
} Elem;

#define INVALID_END_POINT 1
#define BIG_END_POINT 1
#define LITTLE_END_POINT 2


#define METHOD_2

// 字节序
int GetByteOrder()
{
#ifdef METHOD_2
	printf("method 2\n");
	uint32_t aa;	
	aa = 0x1;    
	// aa = htonl(0x1);    // 转为网络字节序模拟大端
	uint8_t *p = (uint8_t *)&aa;
	if (p[0] == 0x1) {
		return LITTLE_END_POINT;
	} else if (p[3] == 0x1) {
		return BIG_END_POINT;
	} else {
		return INVALID_END_POINT;
	}

#else
	printf("method 1\n");
	Elem aa;	
	// aa.no = htonl(0x1);    转为网络字节序模拟大端
	aa.no = 0x1;    
	if (aa.arr[0] == 0x1) {
		return LITTLE_END_POINT;
	} else if (aa.arr[3] == 0x1) {
		return BIG_END_POINT;
	} else {
		return INVALID_END_POINT;
	}
	
#endif

}


int main (int argc, char* argv[]) {
	int res = GetByteOrder();
	if (LITTLE_END_POINT == res) {
		printf("little end point\n");
	} else if (BIG_END_POINT == res) {
		printf("big end point\n");
	} else {
		abort();
	}	
}
