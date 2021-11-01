#include <stdio.h>
int val1 = 50000;

void func1()
{
	printf("new func %s\n", __func__);
}


void func2()
{
	printf("new func %s\n", __func__);
}

void func3()
{
	printf("new func %s\n", __func__);
}

void func_inc_val1()
{
	val1 += 1;
}

void func_dec_val1()
{
	val1 -= 1;
}
