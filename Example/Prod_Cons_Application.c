#include<stdio.h>
#include<unistd.h>
#include<pthread.h>
#define buffSize 1000
struct {
	pthread_mutex_t mutex;
	int buff[buffSize];
	int producedCount;
	int consumedCount;
}shared = { PTHREAD_MUTEX_INITIALIZER };

void* produce(void* arg);
void* consume(void* arg);
void main() {
	int prodThreads, consThreads, i;
	printf("\nEnter the no of Producers : ");
	scanf(" % d", &prodThreads);
	printf("\nEnter the no of Consumers : ");
	scanf("% d", &consThreads);
	int producerArr[prodThreads], consumerArr[consThreads];
	pthread_t producer[prodThreads], consumer[consThreads];
	pthread_setconcurrency(prodThreads + consThreads);

	for (i = 0; i < prodThreads; i++){
		producerArr[i] = 0;
		pthread_create(&producer[i], NULL, produce, &producerArr[i]);
	}

	for (i = 0; i < consThreads; i++){
		consumerArr[i] = 0;
		pthread_create(&consumer[i], NULL, consume, &consumerArr[i]);
	}

	for (i = 0; i < prodThreads; i++){
		pthread_join(producer[i], NULL);
		printf("\nThe Producer(% d) produced: [% d] Items", i, producerArr[i]);
		sleep(1);
	}
	printf("\n");

	for (i = 0; i < consThreads; i++){
		pthread_join(consumer[i], NULL);
		printf("\nThe Consumer(% d) Consumed: [% d] Items", i, consumerArr[i]);
		sleep(1);
	}
}

void* produce(void* arg){
	while (1)
	{
		pthread_mutex_lock(&shared.mutex);
		if (shared.producedCount < buffSize){
			shared.producedCount++;
			*((int*)arg) += 1;
			pthread_mutex_unlock(&shared.mutex);
		}
		else{
			pthread_mutex_unlock(&shared.mutex);
			return NULL;
		}
	}
}

void* consume(void* arg){
	while (1){
		pthread_mutex_lock(&shared.mutex);
		if (shared.consumedCount < shared.producedCount){
			shared.consumedCount++;
			*((int*)arg) += 1;
			pthread_mutex_unlock(&shared.mutex);
		}else{
			pthread_mutex_unlock(&shared.mutex);
			if (shared.consumedCount >= buffSize)
				return NULL;
		}
	}
}