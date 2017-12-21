#include <iostream>
using namespace std;
int main()
{
	int a,b,c,d,max;
	cout<<"请输入三个数字";
	cin>>a>>b>>c;
	max=(d=a>=b?a:b)>=c?d:c;
	cout<<"最大值为:"<<max<<endl;
	return 0;
}
