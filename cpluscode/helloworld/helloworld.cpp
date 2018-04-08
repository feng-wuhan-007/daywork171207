#include <stdlib.h>
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
}
