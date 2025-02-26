/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Tue 25 Feb 2025 11:28:56 AM CST
 @ File Name	: t1.cpp
 @ Description	: 
 ************************************************************************/
#include <asm-generic/errno-base.h>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <sched.h>
#include <vector>
#include <string>
#include <algorithm>
#include <climits>
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <unistd.h>

using namespace std;

int main(int argc, char *argv[]) {
	
	std::cout << "=========== test ==============" << std::endl;
	errno = EACCES;
	perror("i am tmark");
	
	string str(strerror(EBADF));
	std::cout << str << std::endl;
	str.append("gaoxiao");
	std::cout << str << std::endl;

	pid_t pid = getpid();
	std::cout << pid << std::endl;


	return 0;
}
