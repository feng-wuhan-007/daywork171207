#include <iostream>
using namespace std;
int main()
{
	int *ptr=NULL;
	int var=1;
	ptr=&var;
	cout<<ptr<<endl;
	cout<<"ptr-->var="<<*ptr<<endl;
	cout<<"var="<<var<<endl;
	return 0;

}
