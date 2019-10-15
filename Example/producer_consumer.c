// Producer-consumer problem.
// (c) 2008 Pedro Fortuny Ayuso.
// This program gets three parameters (threshold, producers, consumers)
// and creates 'producers' number of producers, 'consumers' number of
// consumers and all of them keep doing their job. Consumers are unable
// to 'eat' if a counter is 0 and producers are unable to produce if
// the same counter is == threshold. A consumer decreases the counter
// and a producer increases it [when they do their job].

// The status of the system is visualized with a series of dots and the
// position of the cursor (your terminal may not 
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/file.h>
#include <unistd.h>
#include <math.h>


// I tend to think this structure is more useful than naming
// conventions. This way you associate an intrinsic mutex to
// any variable, uniquely.
typedef struct {
  pthread_mutex_t mutex;
  int value;
} mutex_int;

// OK, these are globals, could me made non-globals, but this is
// just an exercise.
int threshold;
pthread_t full_mutex, empty_mutex;
pthread_cond_t full, empty, cond;


void *producer(void *count){

  int sl = 0;
  mutex_int *c;
  c = (mutex_int *)count;
  for(;;){

    if(pthread_mutex_lock(&c->mutex) != 0){
      perror("Unable to lock a mutex");
      pthread_exit(NULL);
    }

    // producers wait for a "not full" message from either
    // the main loop or the consumers
    while(c->value == threshold)
      if(pthread_cond_wait(&full, &c->mutex) != 0){
	perror("Unable to wait upon condition");
	pthread_exit(NULL);
      }
    
    sl = ++c->value;
    printf(".");
    // use this line if your terminal does not move the cursor
    //printf("[%i]\n", sl);
    fflush(stdout);
    
    // after producing, this producer broadcasts a "not empty"
    // message to all consumers;
    if((pthread_cond_broadcast(&empty) != 0) ||
       (pthread_mutex_unlock(&c->mutex) != 0)){
      perror("Unable to broadcast or unlock");
      pthread_exit(NULL);
    }

    usleep(log(sl+1) * random()/3000 + 1);
  }

}



void *consumer(void *count){

  int sl = 0;
  mutex_int *c;
  c = (mutex_int *)count;
  for(;;){

    if(pthread_mutex_lock(&c->mutex) != 0){
      perror("Unable to lock a mutex");
      pthread_exit(NULL);
    }

    // consumers wait for a "not empty" message from producers
    // Notice that we MUST check the value every time we are
    // notified
    while(c->value == 0)
      if(pthread_cond_wait(&empty, &c->mutex) != 0){
	perror("Unable to wait upon condition");
	pthread_exit(NULL);
      }

    sl = --c->value;
    printf("\b \b");
    // use this line if your terminal does not move the cursor
    //printf("[%i]\n", sl);
    fflush(stdout);

    // consumer broadcast a "not full" message to producers
    if((pthread_cond_broadcast(&full) != 0) ||
       (pthread_mutex_unlock(&c->mutex) != 0)){
      perror("Unable to broadcast or to unlock");
      pthread_exit(NULL);
    }

    usleep(log((threshold - sl + 1)) * random()/3000);
  }
}



int main(int argc, char *argv[]) {

  //sranddev();
  
  if(argc < 4){
    printf("Usage: ./producer_consumer threshold producers consumers\n");
    exit(1);
  }

  threshold = atoi(argv[1]);
  int producers = atoi(argv[2]);
  int consumers = atoi(argv[3]);
  
  if(threshold == 0 || producers <= 0 || consumers <= 0){
    printf("Please, threshold and producers and cosumers must be positive\n");
    exit(1);
  }

  if((pthread_cond_init(&full, NULL) != 0) ||
     (pthread_cond_init(&empty, NULL) !=0)){
    perror("Unable to init conditions");
    exit(1);
  }

  mutex_int *count = (mutex_int *)calloc(1,sizeof(mutex_int));
  if(count == NULL) {
    perror("Unable to malloc");
    exit(1);
  } 

  count->value = 0;

  if(pthread_mutex_init(&count->mutex, NULL) != 0){
    perror("Unable to initiate mutex");
    exit(1);
  }

  char *sp = (char *)calloc(threshold+3,sizeof(char));
  int i;
  printf("min->|");
  for(i=0; i<threshold; i++)
          sp[i]=' ';
  sp[threshold]='|';
  printf("%s",sp);
  printf("<-max");
  for(i=0; i<=threshold+5; i++)
          printf("\b");
  for(i = 0; i< producers; i++){
    pthread_t t;
    if(pthread_create(&t, NULL, producer, (void *) count) != 0){
      perror("Unable to create thread");
      exit(1);
    }
  }
  
  for(i = 0; i< consumers; i++){
    pthread_t t;
    if(pthread_create(&t, NULL, consumer, (void *) count) != 0){
      perror("Unable to create thread");
      exit(1);
    }
  }

  if(pthread_cond_broadcast(&full) != 0){
    perror("Unable to broadcast");
    exit(1);
  };
  
  pthread_exit(NULL);

}
