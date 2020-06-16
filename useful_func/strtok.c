/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Tue 16 Jun 2020 10:22:20 AM CST
 @ File Name	: strtok.c
 @ Description	: 
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void printArray(char * str, int size)
{
	for(int idx = 0; idx < size; ++idx) {
		if (str[idx] == '\0') {
			printf("\\0"); 
		} else {
			putchar(str[idx]);
		}
	}
	putchar('\n');
}

int main(int argc, char *argv[]) {
	if (3 != argc) {
		fprintf(stderr, "usage : %s str delimeter\n", __FILE__);
		exit(EXIT_FAILURE);
	}

	char * str = argv[1];
	int len = strlen(str); 
	char * delim = argv[2];
	printf("begin : ");
	printArray(str, len);
	
	for (int idx = 1; ;str = NULL, ++idx) {
		char *res = strtok(str, delim);	
		if (NULL == res) {
			printf("strtok end.\n");
			break;
		}

		printf("%d.%s\n", idx, res);
	}
	
	str = argv[1];
	printf("end : ");
	printArray(str, len);

	return 0;
}
