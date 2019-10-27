#include <stdio.h>
#include<pthread.h>

pthread_mutex_t lock;

void produce(void *arg){
    pthread_mutex_lock(&lock);
    //do something
    pthread_mutex_unlock(&lock);
    return NULL;
}
void consume(void *arg){
    pthread_mutex_lock(&lock);
    //do something
    pthread_mutex_unlock(&lock);
    return NULL;
}
