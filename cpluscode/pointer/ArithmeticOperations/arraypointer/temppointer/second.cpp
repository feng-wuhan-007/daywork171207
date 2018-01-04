#include <iostream>
using namespace std;
const int MAX=3;
int main()
{
	int var[MAX]={10,100,200};
	int *ptr[MAX];
	for(int i=0;i<MAX;i++)
	{
		ptr[i]=&var[i];

	}
	for(int j=0;j<MAX;j++)
	{
		cout<<*ptr[j]<<endl;
	}
	cout<<"ptr"<<ptr[2]<<endl;
	cout<<"var"<<var<<endl;
	return 0;
}
