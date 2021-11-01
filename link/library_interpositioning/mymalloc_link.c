/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Mon 01 Nov 2021 06:02:35 PM CST
 @ File Name	: mymalloc_link.c
 @ Description	: 
 ************************************************************************/
#include <stdio.h>
#include <malloc.h>

void *__real_malloc(size_t size);
void __real_free(void *ptr);


void *__wrap_malloc(size_t size) {
	void * res = __real_malloc(size);
	fprintf(stdout, "malloc(%ld) == %p\n", size, res);
	return res;
}

void __wrap_free(void *ptr) {
	__real_free(ptr);
	fprintf(stdout, "free(%p)\n", ptr);
}

