Src = $(wildcard *.c)
Res = $(patsubst %.c, %, $(Src))

all:$(Res)

$(Res):%:%.c
	gcc -Wl,-rpath,/home/tmark/code/C/SystemFunc/internet/wrap -L /home/tmark/code/C/SystemFunc/internet/wrap $< -lwrap -o $@ -Wall 

clean:
	-rm -rf $(Res) *.g

.PHONY:all







