#ifndef __TEST_H__
#define __TEST_H__
#include <iostream>
using namespace std;
class Test
{
public:
	Test()
	{
	cout <<"Test gouzao function"<<endl;
	}	
	
	~Test()
	{
	cout <<"Test xigou function"<<endl;
	}
	void print1();
	void fi();
};
/*void Test::fi()
{
	cout<<"feng"<<endl;
}*/
#endif

