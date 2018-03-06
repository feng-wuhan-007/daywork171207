#include <iostream>
#include <string>
using namespace std;


class Pc
{
	public:
		int a ;
		int b;
	string s ;	
	static int ii;
};
namespace fi
{
   	class Box
{
	public:
	static	int length;
		int height;
	static	int method(int i);
		 
};
int Box:: method(int i)
	{
		length=i;
		cout << "out method=" << i <<endl ;
	}
int Box::length=0;
	
}