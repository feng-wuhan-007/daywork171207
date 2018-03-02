#include <iostream>
#include "test2.h"
int main()
{
	int a;
	Box ss;
	std::cout << "please input" <<std::endl;
	std::cin >> a;
	std::cout <<"out1="<<a;
	ss.method();
	ss.length=10;
	std::cout <<"out2="<<ss.length; 
//	ss.length=10;
} 