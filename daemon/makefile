Src = $(wildcard *.c)
Res = libdaemon.so

all:$(Src)
	gcc -fpic -shared $^ -o $(Res)

install:
	cp ./daemon.h /usr/local/include	
	cp ./libdaemon.so /usr/local/lib

clean:
	-rm -rf $(Res)

.PHONY:all clean install



