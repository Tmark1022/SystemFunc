#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

int main(int argc, char *argv[]) {
	int idx = 0;		
	alarm(1);
	while(1) {	
		printf("%d\n", ++idx);
	}

	return 0;
}
