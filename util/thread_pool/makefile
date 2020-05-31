Src = $(wildcard *.c)
Header = thread_pool.h
Res = libthread_pool.so

all:$(Src)
	gcc -fpic -shared $^ -o $(Res) -pthread

install:
	cp $(Header) /usr/local/include	
	cp $(Res) /usr/local/lib

clean:
	-rm -rf $(Res)

.PHONY:all clean install



