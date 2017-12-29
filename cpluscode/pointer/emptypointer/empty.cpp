#include <iostream>
using namespace std;
int main()
{
	int *ptr;
	cout<<ptr<<endl;
	int var=1;
	ptr=&var;
	cout<<ptr<<endl;
	cout<<"ptr-->var="<<*ptr<<endl;
	cout<<"var="<<var<<endl;
	return 0;

}
