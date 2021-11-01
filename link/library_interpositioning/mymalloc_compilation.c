/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Mon 01 Nov 2021 05:47:44 PM CST
 @ File Name	: mymalloc_compilation.c
 @ Description	: 
 ************************************************************************/
#include <stdio.h>
#include <malloc.h>

void *mymalloc(size_t size) {
	void * res = malloc(size);
	fprintf(stdout, "malloc(%ld) == %p\n", size, res);
	return res;
}

void myfree(void *ptr) {
	free(ptr);
	fprintf(stdout, "free(%p)\n", ptr);
}

