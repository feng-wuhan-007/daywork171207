#include <iostream>
using namespace std;
#include <stdio.h>
#include "test.h"
const  int ui = 6;
void func(const int x)
{
    //x = 6;
    printf("func() x = %d\n",x);
}

int main()
{
   // F f;
   // f.Test();
   // SubF subf;
    //****************
    F *pp = new SubF();
    pp->Test();
    pp->say();
    delete pp;
    SubF subf;
    subf.say(2);
    //subf.say();
   // A a;
   // A *p = new A;
   // SubA suba;
    //suba.Display();
   // suba.printg();    protected method
   // delete p;
   C c;
   c.Display();
    func(12);
    printf("welcome ui = %d\n",ui);
    return 0;
    
}
