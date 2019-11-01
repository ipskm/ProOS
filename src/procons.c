#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

//กำหนดค่าตัวแปร
#define BUFFER_SIZE 1000 //ขนาดบัฟเฟอร์
#define REQUEST 1000     //จำนวนผลิต
#define PROD 10            //จำนวนตัวผลิต
#define CONS 10            //จำนวนตัวลบออก
#define THREAD PROD + CONS
#define TRUE 1
#define FALSE 0

pthread_mutex_t mutex;
pthread_t tid;
pthread_attr_t attr;
sem_t full, empty;

void *append_(void *param);
void *remove_(void *param);

/* sem_post algorithm:
    mutex_lock  sem_t->mutex
    sem_t->value++
    mutex_unlock sem_t->mutex
 
   sem_wait algorithn:
    mutex_lock sem_t->mutex
    while (sem_t->value > 0) {
        mutex_unlock sem_t->mutex
        sleep... wake up
        mutex_lock sem_t->mutex
    }
    sem_t->value--
    mutex_unlock sem_t->mutex
*/

// โครงสร้างข้อมูลแบบคิว
struct queue
{
    int *items;  // array to store queue elements อาเรย์ใช้เก็บสมาชิกของคิว
    int maxsize; // maximum capacity of the queue ขนาดสูงสุดที่จุได้ของคิว
    int front;   // front points to front element in the queue (if any) จุดชี้สมาชิกตัวแรกของคิว
    int rear;    // rear points to last element in the queue จุดชี้สมาชิกตัวสุดท้าบของคิว
    int size;    // current capacity of the queue ขนาดของคิวล่าสุด
};

// Utility function to initialize queue ฟังก์ชันสร้างคิวใหม่
struct queue *newQueue(int size)
{
    struct queue *pt = NULL;
    pt = (struct queue *)malloc(sizeof(struct queue));

    pt->items = (int *)malloc(size * sizeof(int));
    pt->maxsize = size;
    pt->front = 0;
    pt->rear = -1;
    pt->size = 0;

    return pt;
}

// Utility function to return the size of the queue ฟังก์ชันตรวจสอบขนาดของคิว
int size(struct queue *pt)
{
    return pt->size;
}

// Utility function to check if the queue is empty or not ฟังก์ชันตรวจสอบว่าคิวว่างหรือม่
int isEmpty(struct queue *pt)
{
    return !size(pt);
}

// Utility function to return front element in queue ฟังก์ชันส่งค่าตัวแรกสุดของคิว
int front(struct queue *pt)
{
    if (isEmpty(pt))
    {
        printf("UnderFlow\nProgram Terminated\n");
        // exit(EXIT_FAILURE);
    }

    return pt->items[pt->front];
}

// Utility function to add an element x in the queue ฟังก์ชันเพิ่มค่า x ลงในท้ายคิว
void add_item(struct queue *pt, int x)
{
    if (size(pt) == pt->maxsize)
    {
        printf("OverFlow\nProgram Terminated\n");
        // exit(EXIT_FAILURE);
    }

    printf("Adding %d\t", x);

    pt->rear = (pt->rear + 1) % pt->maxsize; // circular queue
    pt->items[pt->rear] = x;
    pt->size++;

    printf("front = %d, rear = %d\n", pt->front, pt->rear);
}

// Utility function to remove element from the queue ฟังก์ชันลบตัวแรกออกจากหัวคิว
void remove_item(struct queue *pt)
{
    if (isEmpty(pt)) // front == rear
    {
        printf("UnderFlow\nProgram Terminated\n");
        // exit(EXIT_FAILURE);
    }

    printf("Removing  %d\t", front(pt));

    pt->front = (pt->front + 1) % pt->maxsize; // circular queue
    pt->size--;

    printf("front = %d, rear = %d\n", pt->front, pt->rear);
}

void *append_(void *param)
{
    struct queue *pt;
    int x = 0;
    while (1)
    {
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        add_item(pt, x);
        pthread_mutex_unlock(&mutex);
        sem_post(&full);
    }
}
void *remove_(void *param)
{
    struct queue *pt;
    int x = 0;
    while (1)
    {
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        remove_item(pt);
        pthread_mutex_unlock(&mutex);
        sem_post(&full);
    }
}

// main function ฟังก์ชันหลัก
int main(int argc, char *argv[])
{
    struct queue *pt = newQueue(BUFFER_SIZE);
    pthread_mutex_init(&mutex, NULL);
    sem_init(&full, 0, 0);
    sem_init(&empty, 0, 0);
    pthread_attr_init(&attr);

    for(int i = 0;i < PROD; i++){
        pthread_create(&tid,&attr,append_,NULL);
    }
    for(int i = 0;i < CONS; i++){
        pthread_create(&tid,&attr,remove_,NULL);
    }
    for(int i = 0;i <= THREAD; i++){
        pthread_join(&tid,NULL);
    }
}