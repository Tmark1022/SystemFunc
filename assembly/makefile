Src = $(wildcard *.c)
Res = $(patsubst %.c, %, $(Src))

all:$(Res)

$(Res):%:%.c
	gcc -Og $< -o $@ -Wall -g

clean:
	-rm -rf $(Res)

.PHONY:all

