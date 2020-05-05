/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Tue 05 May 2020 10:47:46 AM CST
 @ File Name	: rand.c
 @ Description	: 测试随机数
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	int seed = 11;
	unsigned int state = 0;
	srand(seed);
	for (int i = 1; i <= 10; ++i) {

		int res = rand_r(&state);		// rand_r使用state来控制其下一个随机数， 如果一直保持state不变， 那个rand_r的值也不变

		printf("no.%d, rand = %d, state = %d\n", i, res, state);	
		
		// reset variable state
		state = 0;
	}
	
	return 0;
}
