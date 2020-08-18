Src = $(wildcard *.c)
Header = $(wildcard *.h)
Res = libinet_wrap.so

all:$(Src)
	gcc -fpic -shared $^ -o $(Res)

install:
	cp $(Header) /usr/local/include	
	cp $(Res) /usr/local/lib

clean:
	-rm -rf $(Res)

.PHONY:all clean install

