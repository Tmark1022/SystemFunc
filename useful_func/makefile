Src = $(wildcard *.c)
Res = $(patsubst %.c, %, $(Src))

all:$(Res)

$(Res):%:%.c
	gcc $< -o $@ -Wall -std=c99
	gcc $< -o $@.g -Wall -g

clean:
	-rm -rf $(Res) *.g

.PHONY:all







