Src = $(wildcard *.c)
Header = queue.h
Res = libqueue.so

all:$(Src)
	gcc -fpic -shared $^ -o $(Res)

install:
	cp $(Header) /usr/local/include	
	cp $(Res) /usr/local/lib

clean:
	-rm -rf $(Res)

.PHONY:all clean install



