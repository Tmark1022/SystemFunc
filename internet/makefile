Src = $(wildcard *.c)
Res = $(patsubst %.c, %, $(Src))

all:$(Res)

$(Res):%:%.c
	gcc -Wl,-rpath,./wrap -L ./wrap $< -lwrap -o $@ -Wall -pthread -g -lrt -linet_wrap

clean:
	-rm -rf $(Res) *.g

.PHONY:all







