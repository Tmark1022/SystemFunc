/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Fri 19 Jun 2020 04:35:36 PM CST
 @ File Name	: class_sizeof.cpp
 @ Description	: 
 ************************************************************************/
// 修改padding 大小
#pragma pack(1)

#include <iostream>

using std::cin;
using std::cout;
using std::endl;



// empty类大小是1不是0， 为了能标记具体的对象（每个对象地址都不同， 如下面a, b 地址差1）
class Empty { };

void f()
{
	Empty a, b;
	if (&a == &b) cout << "impossible: report error to compiler supplier";

	Empty* p1 = new Empty;
	Empty* p2 = new Empty;
	if (p1 == p2) cout << "impossible: report error to compiler supplier";

	cout << &a << " " << &b << " " << p1 << " " << p2 << endl;
}

// 有一个指针的大小（指向虚函数表）
class A {
public:
	virtual void foo() {};
};


// 一个short， 一个int， 一个指针， 最后还要进行alignment 和 padding 
class B {
private : 
	short b;
	int a;
	virtual void foo() {};
};



int main(int argc, char *argv[]) {

	A a;	
	B b;
	cout << sizeof(A) << " " << sizeof(a) << endl;
	cout << sizeof(B) << " " << sizeof(b) << endl;

	f();

	return 0;
}
