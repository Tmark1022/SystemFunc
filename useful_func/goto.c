/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Wed 27 May 2020 05:32:09 PM CST
 @ File Name	: goto.c
 @ Description	: 
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

	int i = 1;
	int sum = 0;

Start:	
	printf("%d\n", i);
	sum += i++;
	if (i <= 10) {
		goto Start;
	} else {
		goto End;
	}
	
	printf("skip, end haha \n");	

End:
	printf("sum is %d\n", sum);

	return 0;
}
