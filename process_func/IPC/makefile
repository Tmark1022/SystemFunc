Src = $(wildcard *.c)
Res = $(patsubst %.c, %, $(Src))

all:$(Res)

$(Res):%:%.c
	gcc $< sys_err.o -o $@  -g

clean:
	-rm -rf $(Res) *.g

.PHONY:all







