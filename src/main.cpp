/*
 Include Libraly
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

//define pthread function
pthread_t *producers;
pthread_t *consumers;

//define semaphore variable
sem_t buf_mutex, empty_count, fill_count;

int *buf, buf_pos = -1, prod_count, con_count, buf_len, req;

int produce(pthread_t self)
{
	int i = 0;
	int p = 0;
	while (!pthread_equal(*(producers + i), self) && i < prod_count)
	{
		i++;
	}
	printf("Producer %d produced %d \n", i + 1, p);
	printf("%d",&i);
	return 0;
}

void consume(int p, pthread_t self)
{
	int i = 0;
	while (!pthread_equal(*(consumers + i), self) && i < con_count)
	{
		i++;
	}

	printf("Buffer:");
	for(i=0;i<=buf_pos;++i)
		printf("%d ",*(buf+i));
	printf("\nCurrent buffer len: %d\n", i + 1, p, buf_pos);
}

void *producer(void *args)
{
	for (int i = 0; i < req; i++)
	{
		while (1)
		{
			int p = produce(pthread_self());
			sem_wait(&empty_count);
			sem_wait(&buf_mutex);
			++buf_pos; // critical section
			++req;
			*(buf + buf_pos) = p;
			sem_post(&buf_mutex);
			sem_post(&fill_count);
			sleep(1 + rand() % 3);
		}
	}
	return NULL;
}

void *consumer(void *args)
{
	int c;

	while (1)
	{
		sem_wait(&fill_count);
		sem_wait(&buf_mutex);
		c = *(buf + buf_pos);
		consume(c, pthread_self());
		--buf_pos; // Critical section
		sem_post(&buf_mutex);
		sem_post(&empty_count);
		sleep(1 + rand() % 5);
	}

	return NULL;
}

int main(void)
{

	int i, err;

	srand(time(NULL));

	sem_init(&buf_mutex, 0, 1);
	sem_init(&fill_count, 0, 0);
	//get number of producer thread
	printf("Enter the number of Producers: ");
	scanf("%d", &prod_count);
	producers = (pthread_t *)malloc(prod_count * sizeof(pthread_t)); // --> Send parameter prod_count to create producer thread
																	 //get number of comsumer thread
	printf("Enter the number of Consumers: ");
	scanf("%d", &con_count);
	consumers = (pthread_t *)malloc(con_count * sizeof(pthread_t)); // --> Send parameter con_count to create consumer thread
																	//get value of buffer
	printf("Enter buffer capacity: ");
	scanf("%d", &buf_len);
	buf = (int *)malloc(buf_len * sizeof(int)); // --> Send parameter buf_len to create buffer with size from value
												//get value to produce
	printf("Enter the request to produce : ");
	scanf("%d", &req);
	//initial semaphore
	sem_init(&empty_count, 0, buf_len);
	//create producers thread
	for (i = 0; i < prod_count; i++)
	{
		err = pthread_create(producers + i, NULL, &producer, NULL);
		if (err != 0)
		{
			printf("Error creating producer %d: %s\n", i + 1, strerror(err));
		}
		else
		{
			printf("Successfully created producer %d\n", i + 1);
		}
	}
	//create consumers thread
	for (i = 0; i < con_count; i++)
	{
		err = pthread_create(consumers + i, NULL, &consumer, NULL);
		if (err != 0)
		{
			printf("Error creating consumer %d: %s\n", i + 1, strerror(err));
		}
		else
		{
			printf("Successfully created consumer %d\n", i + 1);
		}
	}
	//join thread producer and consumer thread
	for (i = 0; i < prod_count; i++)
	{
		pthread_join(*(producers + i), NULL);
	}
	for (i = 0; i < con_count; i++)
	{
		pthread_join(*(consumers + i), NULL);
	}

	return 0;
}
