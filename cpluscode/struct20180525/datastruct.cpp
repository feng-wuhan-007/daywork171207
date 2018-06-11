#include <iostream>
#include <string.h>
#include "test.h"
//#include <cstring>
using namespace std;
//#define private public
struct Books
{
char title[50];
char author[50];
char subject[100];
string str;
int hello[5];
int book_id;

}koob;
class A
{
private:
	int feng;
public:
	A()
	{
	cout<<"A gouzao function"<<endl;
	}
	~ A()
	{
	cout<<"A xigou function"<<endl;
	}
};
void print( struct Books book)
{
cout <<"book.title="<<book.title<<endl;
}
void print1( struct Books *book)
{
cout <<"book->title="<<book->title<<endl;
}
void Get(int *p)
{
cout <<"*p="<<*p<<endl;
}

int main()
{
cout<<"welcome to wuhan"<<endl;
///////////////////////////////
int age=20;

Get(&age);
struct Books Book1;
//koob.title="hahaha";
koob.str="hahaha";
koob.hello[0]=1;
koob.hello[1]=2;
koob.hello[2]=3;
koob.hello[3]=4;
koob.hello[4]=5;
cout <<"koob.str="<<koob.str<<endl;
for(int i=0;i<5;++i)
{
cout <<"koob.hello"<<"["<<i<<"]="<<koob.hello[i]<<endl;
}
strcpy(koob.title,"cat");
//Book1
strcpy(Book1.title,"c++ book !!!");
cout <<"Book1.title="<<Book1.title<<endl;
print(Book1);
print1(&Book1);
print(koob);
///////class A
A a;
A *p =new A();
delete p;
//a.feng=10;
/////////////////Test class
Test t1;
t1.print1();
t1.fi();
cout <<"2018.06.03="<<endl;
return 0;
}
