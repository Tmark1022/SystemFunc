Src = $(wildcard *.c)
Res = $(patsubst %.c, %, $(Src))

all:$(Res)

$(Res):%:%.c
	gcc  $< -lqueue -o $@ -Wall -pthread -g

clean:
	-rm -rf $(Res) *.g

.PHONY:all







