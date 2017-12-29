#include <iostream>
using namespace std;
const int MAX=3;
int main()
{
	int var[MAX]={10,100,200};
	int *ptr;
	ptr=&var[MAX-1];
	for(int i=MAX;i>0;i--)
	{
		cout <<ptr<<endl;
		cout <<*ptr<<endl;
		ptr--;
	}
	return 0;	
}
