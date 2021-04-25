#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include<time.h>
#define PRODUCER_NUM 2 
#define CONSUMER_NUM 3 
#define POOL_SIZE 3  
int buffer[POOL_SIZE];    
int head = 0; 
int rear = 0; 
sem_t room_sem;    
sem_t product_sem;   
pthread_mutex_t mutex;
struct tm tm;

int m = 1, n = 20;

void *producer_fun(void *arg)
{
    for (int i = 0; i < 6; i ++)
    {
        sleep(rand()%(n-m+1)+m);
        sem_wait(&room_sem);  
        pthread_mutex_lock(&mutex);
        
        buffer[rear] = 1;
        rear = (rear + 1) % POOL_SIZE;
        time_t t = time(NULL);
        tm = *localtime(&t);
        printf("item%ld produced, at %d-%d-%d %d:%d:%d, and being inserted into buffer; ", (long int)arg + 1,  tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
        printf("there is %d datas into the buffer，left %d into the buffer\n", (rear - head + POOL_SIZE) % POOL_SIZE, POOL_SIZE - ((rear - head + POOL_SIZE) % POOL_SIZE));
        pthread_mutex_unlock(&mutex);
        sem_post(&product_sem);  // product_sem + 1
    }
}

void *consumer_fun(void *arg)
{
    for (int i = 0; i < 4; i ++)
    {
        sleep(rand()%(n-m+1)+m);
        sem_wait(&product_sem);
        pthread_mutex_lock(&mutex);
        head = (head + 1) % POOL_SIZE;
        time_t t = time(NULL);
        tm = *localtime(&t);
        printf("item%ld consumed, at %d-%d-%d %d:%d:%d, Being withdrawn from buffer; ", (long int)arg + 1,  tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
        printf("there is %d into the bufffer，left %d into the buffer\n", (rear - head + POOL_SIZE) % POOL_SIZE, POOL_SIZE - ((rear - head + POOL_SIZE) % POOL_SIZE));
        pthread_mutex_unlock(&mutex);
        sem_post(&room_sem);
    }
}

int main()
{
    pthread_t producer_id[PRODUCER_NUM];
    pthread_t consumer_id[CONSUMER_NUM];
    pthread_mutex_init(&mutex, NULL);   
    int ret = sem_init(&room_sem, 0, POOL_SIZE - 1);  
    if (ret != 0)
    {
        printf("sem_init error");
        exit(0);
    }
    ret = sem_init(&product_sem, 0, 0);
    if (ret != 0)
    {
        printf("sem_init error");
        exit(0);
    }
    for (int i = 0; i < PRODUCER_NUM; i ++)
    {
        ret = pthread_create(&producer_id[i], NULL, producer_fun, (void*)&i);
        if (ret != 0) {
            printf("producer_id error");
            exit(0);
        }
    }
    for (int i = 0; i < CONSUMER_NUM; i ++)
    {
        ret = pthread_create(&consumer_id[i], NULL, consumer_fun, (void*)&i);
        if (ret != 0)
        {
            printf("consumer_id error");
            exit(0);
        }
    }
    for(int i = 0; i < PRODUCER_NUM; i ++)
    {
        pthread_join(producer_id[i],NULL);
    }
    for(int i = 0; i < CONSUMER_NUM; i ++)
    {
        pthread_join(consumer_id[i],NULL);
    }
    exit(0);
}
