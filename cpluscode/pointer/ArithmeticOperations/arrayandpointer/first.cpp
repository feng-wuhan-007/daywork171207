#include <iostream>
using namespace std;
const int MAX=5;
int main()
{
	int var[MAX]={10,100,200};
	for(int i=0;i<MAX;i++)
	{
		*var=i;
	}
	cout<<"var[0]"<<var[0]<<endl;
}
