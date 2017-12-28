#include <iostream>
using namespace std;
int main()
{
	int var=20;
	int *ip=NULL;
	ip=&var;
	cout<<"var="<<var<<endl;
	cout<<"address var="<<ip<<endl;
	cout<<"ip-->var="<<*ip<<endl;
	return 0;
}
