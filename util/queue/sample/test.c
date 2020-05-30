/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Sat 30 May 2020 06:02:08 PM CST
 @ File Name	: test.c
 @ Description	: 
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <queue.h>

int main(int argc, char *argv[]) {

	queue_t que;
	int ret = queue_init(&que, 5);
	printf("queue_init, ret %d\n", ret);
	int choice;
	int *aa;
	queue_print(stdout, &que);

	while (EOF != scanf("%d", &choice)) {
		// 方便测试
		int tmp = 9999;
		aa = &tmp;
		switch (choice) {
			case 1 : 
				aa = malloc(sizeof(int));
				scanf("%d", aa);
				ret = queue_enqueue(&que, aa);
				printf("queue_enqueue, ret %d\n", ret);
				break;
			case 2 : 
				ret = queue_dequeue(&que, (void **)&aa);
				printf("queue_dequeue, ret %d, aa = %d\n", ret, *aa);
				break;
			case 3 : 
				ret = queue_max_size(&que);
				printf("queue_max_size, ret %d\n", ret);
				break;
			case 4 : 
				ret = queue_cur_size(&que);
				printf("queue_cur_size, ret %d\n", ret);
				break;
		}
		queue_print(stdout, &que);

	}	

	ret = queue_destroy(&que);
	printf("queue_destroy, ret %d\n", ret);



	return 0;
}
