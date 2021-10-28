#include <stdio.h>
int val2 = 20;

void func4()
{
	printf("func %s\n", __func__);
}


void func5()
{
	printf("func %s\n", __func__);
}

void func6()
{
	printf("func %s\n", __func__);
}

void func_inc_val2()
{
	val2 += 1;
}

void func_dec_val2()
{
	val2 -= 1;
}
