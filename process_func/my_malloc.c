/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Mon 20 Dec 2021 03:09:48 PM CST
 @ File Name	: my_malloc.c
 @ Description	: 动态内存使用, 配合使用pmap 或 procfs 来查看vma的情况。
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

/***************************************************
* brk 
***************************************************/
void *malloc_brk(size_t size)
{
	if (size <= 0) return NULL;

	// 为了测试效果， 规定最少请求2
	if (size <= 2) return NULL;

	void * ptr = sbrk(size);
	if ((void *)-1 == ptr) {
		perror("sbrk failed.");
		exit(EXIT_FAILURE);
	}
	return ptr;
}

void free_brk(void *ptr)
{
	// do nothing. 
	return ;
}

/***************************************************
* mmap 创建VMA当做虚拟动态内存 
***************************************************/
void *malloc_mmap(size_t size) {
	if (size <= 0) return NULL;

	// 为了测试效果， 规定最少请求2
	// 对于mmap， 可以不用设置这个， 因为分配的虚拟内存总是page 大小的整数倍
	if (size <= 2) return NULL;

	void * ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if ((void *)-1 == ptr) {
		perror("mmap failed.");
		exit(EXIT_FAILURE);
	}
	
	return ptr;
} 

void free_mmap(void *ptr, size_t length) {
	if (NULL == ptr) {
		return ;
	}

	if (-1 == munmap(ptr, length)) {
		perror("munmap failed.");
		exit(EXIT_FAILURE);
	}	
}

/***************************************************
* do it 
***************************************************/

int main(int argc, char *argv[]) {
	size_t cnt;
	void *ptr = NULL;
	void*(*pmalloc)(size_t) = malloc_mmap; 
	void(*pfree)(void *) = free_brk; 
	void(*pfree2)(void *, size_t) = free_mmap; 

	while (scanf("%ld", &cnt) != EOF) {
		ptr = pmalloc(cnt); 	
		printf("ptr is %p\n", ptr);
		if (ptr) {
			char *ch = (char *) ptr;	
			ch[0] = 'b'; 
			ch[1] = '\0';	
			fprintf(stdout, "malloc successfully. %s\n", ch);
		} else {
			fprintf(stdout, "malloc failed.\n");
		}
		
		fprintf(stdout, "press any key to continue...\n");
		getchar();
		// pfree(ptr);
		// pfree2(ptr, cnt);
		ptr = NULL;
	}

	return 0;
}
