#include<stdio.h>
#include<pthread.h>

#define MAX 100000
pthread_mutex_t the_mutex;
pthread_cond_t condc, condp;
int buffer=0;

void *producer(void *ptr){
    int i;
    for(i=1; i<=MAX; i++){
        pthread_mutex_lock(&the_mutex);
        while(buffer != 0) pthread_cond_wait(&condp, %the_mutex);
        buffer = i;
        pthread_cond_signal(&condc);
    }
}
