#include <stdio.h>
#include <iostream>
#include "test.h"
#include "tmp.h"
#include <string>
#include <stdlib.h>
using namespace std;
//char* s1 = "hello sysu";
//char s2[] = "hello world";
int main()
{
    
    string s1;
    string s2 = s1;
    string s3 = "I am Yasuo";
    string s4(10,'a');
    string s5(s4);
    string s6("I am Pil");
    string s7 = string(6,'c');
    cin >> s1;
    cout << s1 << endl;
    cout << s2 << endl;
    cout << s3 << endl;
    cout << s4 << endl;
    cout << s5 << endl;
    cout << s6 << endl;
    cout << "s7 size = " <<s7.size() << endl;
    cout << (s2.empty()? "This string is empty":"This string is not empty") << endl;
    string str("hi sysu");
    for(string::iterator it = str.begin();it != str.end();++it)
    {
        cout << "str =" << *it << endl;

        *it = 'o';
    }
    string s("heaaoolo sdaa ss ");
    cout << s.find("aa",0)<<endl;

   // s3 = s6;
   // cout << s3 << endl;
   // s1 = s4;
   // cout << s4 << endl;
   // cout << s1 << endl;

   // char* p = s1;
   // cout << p << endl;
    //wal = 9;
    printf("hahaha\n");
    printf("wal = %d\n",wal);
    printf("ge = %d\n",ge);
   // system("pause");
    return 0;
}