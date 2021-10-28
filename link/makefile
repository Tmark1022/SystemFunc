all:main 

main:main.c
	gcc $^ -o $@ -Wall -g -ldl -rdynamic

func:func1.c func2.c
	gcc -shared -fpic $^ -o libfunc.so

clean:
	-rm -rf main libfunc.so

.PHONY:all

