mod = main main_compilation main_link main_runtime

all:$(mod)

main:main.c
	gcc $^ -o $@ -Wall -g

main_compilation:main.c mymalloc_compilation.c 
	gcc mymalloc_compilation.c -c -o mymalloc_compilation.o -Wall -g
	gcc -I ./ main.c mymalloc_compilation.o -o $@ -Wall -g

main_link:main.c mymalloc_link.c 
	gcc main.c  mymalloc_link.c -Wl,--wrap=malloc -Wl,--wrap=free -o $@ -Wall -g 


main_runtime:main.c mymalloc_runtime.c
	gcc -shared -fpic mymalloc_runtime.c -o libmymalloc.so -Wall -g -ldl
	gcc main.c -o $@ -Wall -g

clean:
	-rm -rf $(mod)

.PHONY:all

