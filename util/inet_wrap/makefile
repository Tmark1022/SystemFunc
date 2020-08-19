Src = $(wildcard *.c)
Header = $(wildcard *.h)
Res = libinet_wrap.so

all:$(Src)
	-test -x $(Res) && mv $(Res) $(Res).bak
	gcc -fpic -shared $^ -o $(Res)

install:
	cp $(Header) /usr/local/include	
	cp $(Res) /usr/local/lib

clean:
	-rm -rf $(Res) $(Res).bak

.PHONY:all clean install

