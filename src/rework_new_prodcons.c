#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
// สร้าง circular buffer //
struct buffer {
    int size;       //ขนาด
    int start;      //พอยด์เตอร์เริ่มต้น
    int count;      //ชี้ตำแหน่ง
    void **element; //ข้อมูล
};
int  c_remove = 0;
typedef struct buffer buffer_t;
 
void init(buffer_t *buffer, int size) {
    buffer->size = size;
    buffer->start = 0;
    buffer->count = 0;
    buffer->element = malloc(sizeof(buffer->element)*size);     
}
 
int full(buffer_t *buffer) {
    if (buffer->count == buffer->size) { 
        return 1;
    } else {
        return 0;
    }
}
 
int empty(buffer_t *buffer) {
    if (buffer->count == 0) {
        return 1;
    } else {
        return 0;
    }
}
     
void add_item(buffer_t *buffer, void *data) {
    int index;
    if (full(buffer)) {
        printf("Buffer overflow\n");
    } else {
        index = buffer->start + buffer->count++;
        if (index >= buffer->size) {
            index = 0;
        }
        buffer->element[index] = data;
    }
}
 
 
void * remove_item(buffer_t *buffer) {
    void * element;
    if (empty(buffer)) {
        printf("Buffer underflow\n");
    } else {
       /* FIFO implementation */
       element = buffer->element[buffer->start];
       buffer->start++;
       buffer->count--;
       if (buffer->start == buffer->size) {
           buffer->start = 0;
       }
        
       return element;
    }
}
 
int main(int argc, char *argv[]) {
    buffer_t buffer;
    int BUFFER_SIZE = atoi(argv[1]);
    int REQUEST = atoi(argv[2]);
    // int PROD = atoi(argv[1]);
    // int CONS = atoi(argv[2]);
    // int BUFFER_SIZE = atoi(argv[3]);
    // int REQUEST = atoi(argv[4]);
    init(&buffer, BUFFER_SIZE);
    int *data = 0;
    int i;
    for (i = 0; i < REQUEST; i++) {
        printf("add_item: %s\n", data);
        add_item(&buffer, data);
    }
    for (i = 0; i < REQUEST; i++) {
        /* need to cast void pointers to char pointers */
        printf("pop from queue: %d\n", (int *)remove_item(&buffer));
        c_remove++;
        
    }
    printf("%d\n", c_remove);
}