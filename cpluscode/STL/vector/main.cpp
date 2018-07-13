#include <stdio.h>
#include <iostream>
#include <vector>
using namespace std;
#include <set>
#include <list>
#include <map>
#include "test.h"
#include "kobe.h"
inline int A::max()
{
    return (a) > (b) ? (a) : (b) ;
}
 int x[10] ;
 int arr[][4]={{1,2,3,4},{5,6,7,8}};
int main()
{
    printf("2/3 = %d\n",2/3);
    printf("3/2 = %d\n",3/2);
    int ci = 1;
    int cj = 2;
    double di = 6.6;
    double dj = 7.7;
    printf("front ci = %d\n",ci);
    printf("front cj = %d\n",cj);
    printf("front di = %f\n",di);
    printf("front dj = %f\n",dj);
    change(ci,cj);
    change(di,dj);
    printf("ci = %d\n",ci);
    printf("cj = %d\n",cj);
    printf("di = %f\n",di);
    printf("dj = %f\n",dj);
    Base *ps = new SubBase(); 
    ps->fun(166);
    ps->fun();
    
   // ps->fun(1,2);
    SubBase sub;
    sub.fun();
    sub.fun(1,2);
    sub.Base::fun(100);
    sub.Base::fun();
    int (*pc)[4] = arr;
    printf("(*arr[0][3] = %d\n",(*pc)[3]);
    printf("(*pc)[4] size = %d\n",sizeof((*pc)[4]));
    printf("pc size = %d\n",sizeof(pc));
    printf(" = %d\n",*(pc[1]+1));
    printf(" = %d\n",*pc[0]);
    printf("*(pc+0) = %d\n",*(pc+0));
    printf("pc = %d\n",pc);
    printf("pc[0] = %d\n",pc[0]);
    printf("(pc+)[0] = %d\n",(pc+1)[0]);
    AA aa;
    BB bb;
    CC cc;
    DD dd;
    cout << "AA aa size = "<< sizeof(aa)<<endl;
    cout << "BB bb size = "<< sizeof(bb)<<endl;
    cout << "CC cc size = "<< sizeof(cc)<<endl;
    cout << "DD dd size = "<< sizeof(dd)<<endl;
   int ccc = 0;
    int *p = x;
    cout << "p - >x = "<< sizeof(*p)<<endl;
     cout << "p size = "<< sizeof(p)<<endl;
     cout << "&x size = "<< sizeof(&x)<<endl;
     cout << "x[0] = "<< *p<<endl;
    D d(88);
   // C c1;
   // C c(6);
    printf("jijijijiji\n");
    B b(6);
    A ay(10.0,6.06);
    A a(3,5);
    a.printfOney();

    cout << "A::max = "<<a.max()<<endl;
    cout << "main.cpp &a = "<<&a<<endl;
   // list<int> l1;
   // vector<int> v1;
    map<string,int> m1;
    m1["Kobe"] = 100 ;
    m1["James"] = 99 ;
    m1["Curry"] = 98 ;
    //m1.erase("Kobe");
    
    m1.insert(pair<string,int>("Harris",89));
  //  cout << m1["Kobe"] << endl;
   // cout << m1["Jordan"] << endl;
   // cout << m1["Harris"] << endl;
    cout << " hhhhhhhh" << endl;
    map<string,int>::iterator im;
    for(im = m1.begin(); im != m1.end(); im++)
    {
        cout << im->first << " " << im->second << endl;
    }







    set<int> s1;
    s1.insert(9);
    s1.insert(9);
    s1.insert(24);
    cout <<"s1 size ="<<s1.size() << endl; 
    set<int>::iterator it;
    for(it = s1.begin();it != s1.end();it++)
    {
        cout << *it << endl;
    }
    set<int> s2;
    s2.insert(3);
    s2.insert(5);
    set<int>::iterator itt;
    printf("xxxxxxyyyyyy\n");
    for(itt = s2.begin();itt != s2.end();itt++)
    {
        cout << *itt << endl;
    }
    vector<int> v7(5,-1);
    cout << v7[0] << endl;
   // vector<int> ve {1,2,3,4,5};
   // cout << "ve " << endl;
    //ve.insert()
    //cout << vec.front() << endl;
    //cout << vec.back() << endl;
    
    printf("zhazhazha\n");
    return 0;
}