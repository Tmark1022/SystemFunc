all:main main2 

main:main.c func
	gcc main.c -o $@ -Wall -g -ldl -rdynamic

main2:main2.c func
	gcc main2.c ./libfunc.so -o $@ -Wall -g

func:func1.c func2.c
	gcc -shared -fpic $^ -o libfunc.so

clean:
	-rm -rf main main2 libfunc.so

.PHONY:all

