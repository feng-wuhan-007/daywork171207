#ifndef __KOBE_H__
#define __KOBE_H__
#include <stdio.h>
class AA
{
public:
    int a;
};
class BB :virtual public AA
{
public:
    int b;

};
class CC :virtual public AA
{
public:
    int c;
};
class DD :public BB,public CC 
{
public:
    int d;
};
//基类和父类的函数覆盖
class Base
{
public:
    void fun()
    {
        printf("Base::fun()\n");
    }
    //virtual void fun(int n)
    virtual void fun(int n)
    {
        printf("Base::fun(int n) n = %d\n",n);
    }
};
class SubBase : public Base
{
public:
    void fun()
    {
        printf("SubBase::fun()\n");
    }
    void fun(int n)
    {
        printf("SubBase::fun(int n) n = %d\n",n);
    }
     void fun(int n,int m)
    {
        printf("SubBase::fun(int n) n = %d\n",n);
        printf("SubBase::fun(int m) m = %d\n",m);
    }
    void func(int x)
    {
        printf("SubBase::fun(int x) x = %d\n",x);
    }
};
template <typename T>
void change (T& x,T& y)
{
    T temp;
    temp = x;
    x = y;
    y = temp;
}
#endif