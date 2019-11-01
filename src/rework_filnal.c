// C program for array implementation of queue
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

#define BUFF_SZ 10
#define REQ 10

sem_t xmutex;
int p_count = 0, c_count = 0;
// A structure to represent a queue
struct Queue
{
    int front, rear, size;
    unsigned capacity;
    int *array;
};

// function to create a queue of given capacity.
// It initializes size of queue as 0
struct Queue *createQueue(unsigned capacity)
{
    struct Queue *queue = (struct Queue *)malloc(sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
    queue->rear = capacity - 1; // This is important, see the add_item
    queue->array = (int *)malloc(queue->capacity * sizeof(int));
    return queue;
}

// Queue is full when size becomes equal to the capacity
int isFull(struct Queue *queue)
{
    return (queue->size == queue->capacity);
}

// Queue is empty when size is 0
int isEmpty(struct Queue *queue)
{
    return (queue->size == 0);
}

// Function to add an item to the queue.
// It changes rear and size
void add_item(struct Queue *queue, int item)
{
    if (isFull(queue))
        return;
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->array[queue->rear] = item;
    queue->size = queue->size + 1;
    printf("%d enqueued to queue\n", item);
}

// Function to remove an item from queue.
// It changes front and size
int remove_item(struct Queue *queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    int item = queue->array[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size = queue->size - 1;
    return item;
}

// Function to get front of queue
int front(struct Queue *queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    return queue->array[queue->front];
}

// Function to get rear of queue
int rear(struct Queue *queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    return queue->array[queue->rear];
}

void append_()
{
    struct Queue *queue;
    sem_wait(&xmutex);
    for (int i = 0; i < REQ; i++)
    {
        add_item(queue, i);
        p_count++;
    }
    sem_post(&xmutex);
}
void remove_()
{
    struct Queue *queue;
    sem_wait(&xmutex);
    for (int i = 0; i < REQ; i++)
    {
        remove_item(queue);
        c_count++;
    }
    sem_post(&xmutex);
}

// Driver program to test above functions./
int main()
{
    sem_init(&xmutex, 0, 1);
    struct Queue *queue = createQueue(BUFF_SZ);
    /*  add_item(queue, 10); //to add value
        remove_item(queue); //to remove value
        front(queue); //front value
        rear(queue); //rear value
    */
    sem_destroy(&xmutex);
    return 0;
}