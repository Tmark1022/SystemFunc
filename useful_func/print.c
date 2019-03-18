#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef SIZE
#define SIZE 1024
#endif 


int main(int argc, char *argv[]){
	
	char Buf[SIZE];
	memset(Buf, 0, SIZE);
	
	for (int i = 0; i < SIZE; ++i)
		printf("%d ", Buf[i]);
	putchar('\n');
	
	sprintf(Buf, "hello world, i am tmark, size is %d\n", SIZE);


	fprintf(stdout, "the string si %s", Buf); 


	return 0;
}
