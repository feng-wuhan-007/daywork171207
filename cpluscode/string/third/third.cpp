#include <iostream>
#include <string>
using namespace std;
int main()
{
	string http="www.runoob.com";
	cout <<"http.length()="<<http.length()<<endl;
	http.append("C++");
	cout<<"http="<<http<<endl;
	int pos=http.find("C++");
	cout <<"pos"<<pos<<endl;
	http.replace(pos,3,"");
	cout <<http<<endl;
	int first=http.find_first_of(".");
	int last=http.find_last_of(".");
	cout <<"first="<<first<<endl;
	cout <<"last="<<last<<endl;
	cout <<http.substr(first+1,last-first-1)<<endl;
	return 0;
}

