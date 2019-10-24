
#include <pthread.h>
#include <stdio.h>
#include <zconf.h>

#define PRODUCER_COUNT 4
#define CONSUMER_COUNT 4
#define THREAD_COUNT (PRODUCER_COUNT + CONSUMER_COUNT)
#define WORK_ITERS 10000
#define WORK_COUNT 10000

#define QUEUE_SIZE 64

// Let's go the easy way and keep a gap between head and tail when full.
static pthread_mutex_t queue_head_lock = PTHREAD_MUTEX_INITIALIZER;
static volatile int queue_head = 1;

static pthread_mutex_t queue_tail_lock = PTHREAD_MUTEX_INITIALIZER;
static volatile int queue_tail = 0;

static float queue[QUEUE_SIZE];

static void *producer(void *data_ptr);
static void *consumer(void *data_ptr);

int main()
{
    pthread_t threads[THREAD_COUNT];
    int data[THREAD_COUNT];

    int thread_id;
    for(thread_id = 0; thread_id < PRODUCER_COUNT; thread_id++)
    {
        data[thread_id] = thread_id;
        pthread_create(&threads[thread_id], NULL, producer, &data[thread_id]);
    }

    for(; thread_id < THREAD_COUNT; thread_id++)
    {
        data[thread_id] = thread_id;
        pthread_create(&threads[thread_id], NULL, consumer, &data[thread_id]);
    }

    for(int i = 0; i < THREAD_COUNT; i++)
    {
        pthread_join(threads[i], NULL);
    }

    return 0;
}

static inline int advance(volatile int *idx)
{
    int old, new;
    do
    {
        old = *idx;
        new = (old + 1) % QUEUE_SIZE;
    } while(!__sync_bool_compare_and_swap(idx, old, new));
    return old;
}

static inline float do_work(float in)
{
    for(int i = 0; i < WORK_ITERS; i++)
    {
        in += in / 2.0f;
    }
    return in;
}

static void *producer(void *data_ptr)
{
    int thread_id = *(int *)data_ptr;
    printf("[%d] producing\n", thread_id);

    for(int i = 0; i < WORK_COUNT; i++)
    {
        float value = do_work(i);

        while(1)
        {
            pthread_mutex_lock(&queue_head_lock);

            if((queue_head + 1) % QUEUE_SIZE != queue_tail)
                break;

            pthread_mutex_unlock(&queue_head_lock);
            sleep(0);
        }

        queue[queue_head] = value;
        advance(&queue_head);
        pthread_mutex_unlock(&queue_head_lock);
    }

    printf("[%d] finished producing\n", thread_id);

    return NULL;
}

static void *consumer(void *data_ptr)
{
    int thread_id = *(int *)data_ptr;
    printf("[%d] consuming\n", thread_id);

    float result;

    // instead of poison pill let's just consume exactly what is produced.
    for(int i = 0; i < WORK_COUNT; i++)
    {
        while(1)
        {
            pthread_mutex_lock(&queue_tail_lock);

            if(queue_tail != queue_head)
                break;

            pthread_mutex_unlock(&queue_tail_lock);
            sleep(0);
        }

        int idx = advance(&queue_tail);
        float data = queue[idx];
        pthread_mutex_unlock(&queue_tail_lock);

        result += do_work(data);
        //printf("[%d] consumed %d\n", thread_id, data);
    }

    printf("[%d] finished consuming result=%f\n", thread_id, result);
    return NULL;
}