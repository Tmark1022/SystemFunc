#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>


// 都是定义， 但是在bss段， 在目标文件中并不存在， 所以在链接阶段中并不会报错说redefinition
// bss 段数据， 在程序载入运行时进行初始化， 初始化为0
char arr[10];
int flag;


#endif
