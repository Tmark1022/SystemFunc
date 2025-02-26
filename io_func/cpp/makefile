Src = $(wildcard *.cpp)
Res = $(patsubst %.cpp, %, $(Src))

all:$(Res)

#$(Res):%:%.c
#	g++ $< -o $@ -Wall -g 
#

%:%.cpp
	g++ $< -o $@ -Wall -g -static

clean:
	-rm -rf $(Res)

.PHONY:all

