#include <stdio.h>
int main()
{
	int a=10;
	unsigned char b;
	printf("a.length=%d\n",sizeof(a)/sizeof(int));
	//printf("b.length=%d\n",sizeof(b)/sizeof(unsigned char));
	printf("b.length=%d\n",sizeof(b));
}
