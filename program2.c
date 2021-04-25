#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>

int main(int argc, char **argv){

   int pid;
   pid = fork();
   struct timeval time_start;
   struct timeval time_end;
   float time_use = 0;
   
   if (pid < 0) { // error occurred 
	  fprintf(stderr, "Fork failed!\n");
	  return 1;//exit(-1);
         }else if (pid == 0) { // child process
	  printf("I am the child, return from fork=%d\n", pid); 
	  printf("\n");
	  execlp("/bin/ps", "ps", NULL);
	  gettimeofday(&time_start,NULL);
	  printf("111time_start.tv_sec:%ld\n",time_start.tv_sec);
	  printf("111time_start.tv_usec:%ld\n\n",time_start.tv_usec);
	  execv(argv[1],&argv[1]);
      }else { // parent process 
          wait(NULL);
	  printf("I am the parent, return from fork, child pid=%d\n", pid);
	  printf("\n");
	  gettimeofday(&time_start,NULL);
	  printf("\n");
	  printf("time_start.tv_sec:%ld\n",time_start.tv_sec);
	  printf("time_start.tv_usec:%ld\n\n",time_start.tv_usec);
	  wait(NULL);
	  gettimeofday(&time_end,NULL);
	  printf("\n");
	  printf("time_end.tv_sec:%ld\n",time_end.tv_sec);
	  printf("time_end.tv_usec:%ld\n",time_end.tv_usec);
	  time_use = (time_end.tv_sec - time_start.tv_sec)*1000000 + (time_end.tv_usec - time_start.tv_usec);
	  printf(" %.3f ",time_use);
	  printf("\n");
	  exit(0);
  }
  return 0;
}
