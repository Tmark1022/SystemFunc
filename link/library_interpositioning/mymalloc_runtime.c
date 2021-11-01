/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Mon 01 Nov 2021 06:33:18 PM CST
 @ File Name	: mymalloc_runtime.c
 @ Description	: 
 ************************************************************************/
#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>

void *malloc(size_t size) {	
	void *(*funcp)(size_t) = NULL; 
	funcp = dlsym(RTLD_NEXT, "malloc");
	char *errstr = dlerror();
	if (NULL != errstr) {
		fprintf(stderr, "%s", errstr);
		exit(EXIT_FAILURE);
	}

	void * res = funcp(size);
	// 不能输出到stdout， 有缓冲区时会调用malloc， 导致死循环了
	// fprintf(stdout, "malloc(%ld) == %p\n", size, res);
	fprintf(stderr, "malloc(%ld) == %p\n", size, res);
	return res;
}

void free(void *ptr) {
	void (*funcp)(void *) = dlsym(RTLD_NEXT, "free");
	char *errstr = dlerror();
	if (NULL != errstr) {
		fprintf(stderr, "%s", errstr);
		exit(EXIT_FAILURE);
	}

	funcp(ptr);
	fprintf(stdout, "free(%p)\n", ptr);
}

