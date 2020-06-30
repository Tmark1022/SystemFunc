Src = $(wildcard *.c)
Res = $(patsubst %.c, %, $(Src))

all:$(Res)

$(Res):%:%.c
	gcc $< -o $@ -Wall -g -levent -levent_pthreads

myhttpd_daemon : myhttpd_daemon.c
	gcc $< -ldaemon -o $@ -Wall -g -levent

clean:
	-rm -rf $(Res) *.g

.PHONY:all







