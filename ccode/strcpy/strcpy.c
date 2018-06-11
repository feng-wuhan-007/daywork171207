#include <stdio.h>
char *strcpy(char *strDest,const char *strSrc)
{
	if((strDest == NULL)||(strSrc == NULL))
	{
	printf("empty");
	return NULL;

	}
	char *p = strDest;
	while((*strDest++ = *strSrc++) != '\0');
	return p;
}

int getlen(const char *str)
{
	int len = 0;
	while( *str++ != '\0')
	{
	len++;
	}
	return len;
}
void strccc(char *strDest,const  char *strSrc)
{
        
	
	printf("len1=%d\n",sizeof(strSrc));
}
int main()
{
  char str[]="0123456789abc";
  char str1[]="abcdefghij";
  char strDest[20];
  char strDest1[20];
  char *s = "tttttt";
  char *p = strcpy(strDest,str);
strccc(strDest1,str1);
printf("%s\n",str);
printf("%s\n",strDest);
printf("%s\n",p);
printf("==%d\n",sizeof(str));

printf("*=%d\n",sizeof(s));
printf("*=%p\n",&s);
return 0;
}
