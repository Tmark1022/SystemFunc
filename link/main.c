#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>

int a_data = 1; 
int a_bss; 

void *handle = NULL;
char *error;

void check_dlopen_api_error()
{
	error = dlerror();
        if (error != NULL) {
		fprintf(stderr, "%s\n", error);
		exit(EXIT_FAILURE); 
	}
}

int reload() {	
	if (handle) {
		dlclose(handle);
		check_dlopen_api_error();
		handle = NULL;	
	}

        handle = dlopen("./libfunc.so", RTLD_LAZY);
	check_dlopen_api_error();
	return 1;
}

int main(int argc, char *argv[]) {
	char str[1024];
	while (fgets(str, 1024, stdin)) {	
		// 去掉换行符 str
		str[strlen(str) - 1] = '\0'; 

		if (0 == strcmp(str, "reload")) {
			reload();	
			fprintf(stdout, "reload successfully!\n");
		} else {
			if (NULL == handle) {
				fprintf(stderr, "not loaded libfunc.so yet\n");	
				continue;
			}

			void *res = dlsym(handle, str);
			if (NULL == res) {
				fprintf(stderr, "symbol %s not found\n", str);
				continue;
			}
			
			if (strstr(str, "func")) {
				void (*funp)() = (void (*)())res;
				funp();
			} else if (strstr(str, "val")) {
				int * valp = (int *)res;	
				fprintf(stdout, "%s 's val is %d\n", str, *valp);
				fprintf(stdout, "a_data %lx, a_bss %lx, valp %lx\n", (long)&a_data, (long)&a_bss, (long)valp);
			} else {
				fprintf(stderr, "don't recongnize\n");
			}
		}
	}

	return 0;
}
