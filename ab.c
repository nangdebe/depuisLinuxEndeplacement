#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <wait.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define EMPTY 0
#define FULL 1
#define MUTEX 2

struct buffer
{
    // a shared memory buffer of size 3 declaration
    int stack[3];
    int topPos;
};

void P(int sem_id, int sem_num){
    /* P mutex */   //definition
    struct sembuf p;
    p.sem_flg = 0;
    p.sem_num = sem_num;
    p.sem_op = -1;
    semop(sem_id, &p, 1);
}

void V(int sem_id, int sem_num){
    /* V (mutex)*/   //definition
    struct sembuf v;
    v.sem_flg = 0;
    v.sem_num = sem_num;
    v.sem_op = 1;
    semop(sem_id, &v, 1);
}

int main(int argc, char *argv[]){
    int sem_ID, shm_ID;
    struct buffer *shrMem;
    pid_t comsumer, producer;
    time_t timep;

    // definition of semaphores or /#processes as we may too
    sem_ID = semget((key_t)1875, 3, IPC_CREAT | 0666);
    semctl(sem_ID, EMPTY, SETVAL, 3);
    semctl(sem_ID, FULL, SETVAL, 0);
    semctl(sem_ID, MUTEX, SETVAL, 1);
    // redirecting it or mapping to the memory[buffer]
    shm_ID = shmget(IPC_PRIVATE, sizeof(shrMem), IPC_CREAT | 0644);
    shrMem = (struct buffer *)(shmat(shm_ID, 0, 0));
    // to items into buffer (initializing)
    for (int i = 0; i < 3; i++)
    {
        shrMem->stack[i] = 0;
    }
    shrMem->topPos = -1;

    // debut to create two producers with FOR LOOP
    for (int i = 0; i < 2; i++)
    {
        producer = fork();
        sleep(rand() % 1 + 1);
        if (producer == 0)
        {
            srand((unsigned)getpid());
            shrMem = (struct buffer *)(shmat(shm_ID, 0, 0));
            for (int j = 0; j < 6; j++) {
                P(sem_ID, EMPTY);// P(empty) to check if empty
                sleep(rand() % 1 + 1);//waiting for action to happen
                P(sem_ID, MUTEX);// P(mutex)
                shrMem->stack[++(shrMem->topPos)] = 1;//produces item or data and exit with no error case
                timep = time(NULL); //timing
                printf("On %s a data been produced\n", ctime(&timep));
                printf("Datas into buffer now：%d %d %d，total of %d\n\n", shrMem->stack[0], shrMem->stack[1], shrMem->stack[2], shrMem->topPos + 1);
                fflush(stdout);
                V(sem_ID, FULL);// V(full)
                V(sem_ID, MUTEX);// V(mutex)
            }
            shmdt(shrMem);
            exit(0);
        }
    }

    // creating 3 consumers using FOR LOOP
    for (int i = 0; i < 3; i++)
    {
        comsumer = fork();
        sleep(rand() % 1 + 1);
        if (comsumer == 0)
        {
            srand((unsigned)getpid());
            shrMem = (struct buffer *)(shmat(shm_ID, 0, 0));
            for (int j = 0; j < 4; j++){
                P(sem_ID, FULL);// P(full) //cheicks or waits if full
                sleep(rand() % 1 + 1);// waits for an even to happen 
                P(sem_ID, MUTEX);// P(mutex)
                shrMem->stack[(shrMem->topPos)--] = 0;// takes the data out of buffer
                // finally exits
                timep = time(NULL);
                printf("On %s a data been consumed\n", ctime(&timep));
                printf("Datas into buffer now：%d %d %d，total of: %d\n\n", shrMem->stack[0], shrMem->stack[1], shrMem->stack[2], shrMem->topPos + 1);
                if((shrMem->topPos+1)==0)
                printf("The buffer is Empty!\n\n");
                fflush(stdout);
                V(sem_ID, EMPTY);// V(empty)
                V(sem_ID, MUTEX);// V(mutex)
            }
            shmdt(shrMem);
            exit(0);
        }
    }
    while (wait(0) != -1);// wait for the child process to be completed
    shmdt(shrMem);
    shmctl(shm_ID, IPC_RMID, 0);
    shmctl(sem_ID, IPC_RMID, 0);
    printf("Press Enter to exit!");
    getchar();
    return 0;
}
