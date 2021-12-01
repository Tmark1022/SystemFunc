/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Wed 01 Dec 2021 11:29:59 AM CST
 @ File Name	: brk.c
 @ Description	: 
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

void sig(int signo)
{
	printf("hello %d\n", signo);
}

int main(int argc, char *argv[]) {
	signal(SIGINT, sig);
	printf("pid = %d\n", getpid());
	pause();

	void * ptr = NULL;
	printf("sbrk(0) == %p\n", ptr = sbrk(0));	

	ptr = (char *)ptr + 4096;  
	int res = brk(ptr);
	printf("sbrk(0) == %p\n", sbrk(0));	

	pause();

	return 0;
}
