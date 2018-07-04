#ifndef __TEST_H__
#define __TEST_H__
#include <stdio.h>

class A
{
private:
    /* data */
public:
    A();
    ~A();
    virtual void Display() = 0;
};
class SubA :public A
{
public:
    SubA()
    {

    }
    ~SubA()
    {

    }
    //void Display()
    //{
    //    printf("SubA Display\n");
    //}
protected:
    void printg()
    {
        printf("SubA printg\n");
    }
};
class C :public SubA
{
public:
    C()
    {
         printf("C::C()\n");
    }
    ~C()
    {
         printf("C::~C()\n");
    }
    void Display()
    {
        printf("SubA Display\n");
    }
};

class F 
{
public:
    F()
    {
         printf("F::F()\n");
    }
    virtual ~F()
    {
         printf("F::~F()\n");
    }
    virtual void Test()
    {
        printf("F::Test()\n");
    }
    void say()
    {
        printf("F::say()\n");
    }
};
class SubF :public F
{
public:
    SubF()
    {
         printf("SubF::SubF()\n");
    }
    ~SubF()
    {
         printf("SubF::~SubF()\n");
    } 
    void Test()
    {
        printf("SubF::Test()\n");
    }
     void say(int x)
    {
        printf("SubF::say() x = %d\n",x);
    }
};



#endif