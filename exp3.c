#include <stdio.h>
#include <stdlib.h>
#include<time.h>
int temp = 0, overflow = 0, mutex = 1, underflow = 10;
int wait_protocol(int counter)
{
      return (--counter);
}
int signal_protocol(int counter)
{
      return(++counter);
}
void producer()
{
      mutex = wait_protocol(mutex);
      overflow = signal_protocol(overflow);
      underflow = wait_protocol(underflow);
      temp++;
      printf("\nProduced:\t Element %d\n", temp);
      mutex = signal_protocol(mutex);
}
void consumer()
{
      mutex = wait_protocol(mutex);
      overflow = wait_protocol(overflow);
      underflow = signal_protocol(underflow);
      printf("\nConsumed:\t Element %d\n", temp);
      temp--;
      mutex = signal_protocol(mutex);
}
int main()
{
      int choice;
      printf("\n1.Type 1 to Produce an element\t");
      printf("2.Type 2 to Consume an element\t");
      printf("3.To Quit type 3\t");
      printf("\n");
      while(1)
      {
            printf("\nEnter your choice ===>>\t");
            scanf("%d", &choice);
            switch(choice)
            {
                  case 1: if((mutex == 1) && (underflow != 0))
                          {
                                producer();
                          }
                          else
                          {
                                printf("Sorry! Buffer is Empty!\n");
                          }
                          break;
                  case 2: if((mutex == 1) && (overflow != 0))
                          {
                                consumer();//it checks first if there is none empty
                          }               //then it consume the element produced
                          else
                          {
                                printf("Sorry! Buffer is empty!\n");
                          }
                          break;
                  case 3: exit(0);//in case somebody wants to exit
                          break;
            }
      }
      return 0;
}
