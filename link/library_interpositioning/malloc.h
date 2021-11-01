#ifndef MALLOC_H
#define MALLOC_H
#include <stdio.h>

void *mymalloc(size_t size);
void myfree(void *ptr);

#define malloc(size) mymalloc(size) 
#define free(ptr) myfree(ptr) 

#endif
