Src = $(wildcard *.c)
Res = $(patsubst %.c, %, $(Src))

all:$(Res)

$(Res):%:%.c
	gcc $< -o $@  -g

clean:
	-rm -rf $(Res) *.g

.PHONY:all







