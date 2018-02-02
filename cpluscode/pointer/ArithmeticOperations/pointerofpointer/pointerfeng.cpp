#include <iostream>
using namespace std;
int main()
{
	int var;
	int *ptr;
	int **pptr;
	var=3000;
	ptr=&var;
	pptr=&ptr;
	cout <<"var=" << var <<endl;
	cout <<"*ptr=" << *ptr <<endl;
	cout <<"**pptr=" <<**pptr <<endl;
	cout <<"*pptr=" <<*pptr <<endl;
	return 0;

}
