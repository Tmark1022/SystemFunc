/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Wed 24 Nov 2021 05:39:17 PM CST
 @ File Name	: return_big_object.c
 @ Description	: 探究函数返回大容量object的汇编实现
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int cnt = 1;
// the size of Object is 48
typedef struct {
	char ch; 	
	int i;	
	long l;
	float f;
	double d;
	char str[10];
}Object; 


// 当返回大对象数据时，返回值%rax返回的是在main栈分配的临时变量【存储大对象】的起始地址;
// 传参时， func隐含传递指向main栈中分配的临时变量空间的起始地址， 放在%rdi中进行传递， 故disassembler后， 看到的func函数其实是两个形参的
//
// 因为返回大对象数据时，需要在调用方的栈中开辟临时变量空间， 且需要进行数据拷贝， 故编程时， 为了提高程序执行效率， 尽量避免返回大数据变量对象。
Object func(long a)		
{
	++cnt;			// 让编译器不能把func(8)函数的调用优化掉
	Object obj;	
	obj.l = a;
	return obj;
}

int main(int argc, char *argv[]) {
	func(8);	
	Object cc = func(24);		// 编译器将 tmp = func(24), cc = tmp 优化为直接 cc = func(24), 避免使用临时变量存储func返回值;
	return 0;
}
