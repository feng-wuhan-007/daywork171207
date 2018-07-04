/*#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
int main()
{
  pid_t child_pid;
  int count = 0;
//create a child pid
child_pid = fork();
if(child_pid == 0){
	printf("child pid\n");
        count++;
	exit(0);
}else{
printf("father pid\n");
count++;
sleep(2);
}
  printf("count=%d\n",count);
  return 0;
}*/
#include <iostream>
using namespace std;
#include <vector>

int main()
{
  vector<int> v1;
  
  char *const s1="string";
  //*s1 = 'q';
  //s1="abc";    error
  //*s1='w';     error
  cout<<s1<<endl;
  const char *s2="string";
  s2="nba";
  cout<<s2<<endl;
  char *s3="string";
  //*s3='k';     error
  cout<<s3<<endl;
  return 0;

}
