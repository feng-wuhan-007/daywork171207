#ifndef __TEST_H__
#define __TEST_H__
#include <iostream>
using namespace std;
#include <stdio.h>
class A
{
public:
    //A(){}
    A(int a,int b):a(a),b(b){}
    A(double x = 0,double y = 0)
    {
        //printf("x = %f\n",x);
        cout << "x = " << x << endl;
        cout << "y = " << y << endl;
    }
    int max();
    void printfOney()
    {
        cout <<"this->a =" << this->a <<endl;
        cout <<"(*this).a  =" << (*this).a <<endl;
        cout <<"this =" << this <<endl;
    }
private:
    int a;
    int b;


};

class B :public A
{
private:
    /* data */
public:
    B(int x)
    {
        cout << "B: public A x = "<<x<<endl;
    }
    ~B()
    {

    }
};

class C
{
private:
    /* data */
public:
    C(int x )
    {
        cout << "C::C()" << endl;
    }
    ~C()
    {

    }
};
class D :public C
{
private:
    /* data */
public:
    D(int j):C(j)
    {
        cout << "D::D() j = " <<j <<endl;
    }
    ~D()
    {

    }
};








#endif