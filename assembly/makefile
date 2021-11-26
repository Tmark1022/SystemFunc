Src = $(wildcard *.c)
Res = $(patsubst %.c, %, $(Src))
Res2 = $(patsubst %.c, %_static, $(Src))

all:$(Res)

$(Res):%:%.c
	gcc -Og $< -o $@ -Wall -g
	gcc -static -Og $< -o $@_static -Wall -g

clean:
	-rm -rf $(Res) $(Res2)

.PHONY:all

