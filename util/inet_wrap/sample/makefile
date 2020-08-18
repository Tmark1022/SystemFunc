Src = $(wildcard *.c)
Res = $(patsubst %.c, %, $(Src))

all:$(Res)

$(Res):%:%.c
	gcc $< ../libinet_wrap.so  -o $@ -Wall -g

clean:
	-rm -rf $(Res) *.g

.PHONY:all







