#include <iostream>
using namespace std;
const int MAX=4;
int main()
{
	char * string="abcdef";
	const char *names[MAX]={
	"zara ali",
	"hina ali",
	"nuha ali",
	"sara ali",
	};
	for(int i=0;i<MAX;i++)
	{
		cout <<"value of names["<<i<<"]=";
		cout <<names[i]<<endl;
		cout <<*(names[i]+1)<<endl;
		
	}
	cout<<"string="<<string<<endl;
	//cout<<"*string="<<*string<<endl;
	return 0;
}
