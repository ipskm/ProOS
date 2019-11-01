#include <stdio.h>
#include <stdlib.h>
 
struct buffer {
    int size;
    int start;
    int count; 
    void **element;
};
 
typedef struct buffer buffer_t;
 
void init(buffer_t *buffer, int size) {
    buffer->size = size;
    buffer->start = 0;
    buffer->count = 0;
    buffer->element = malloc(sizeof(buffer->element)*size);
    /* allocated array of void pointers. Same as below */
    //buffer->element = malloc(sizeof(void *) * size);
     
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
     
void push(buffer_t *buffer, void *data) {
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
 
 
void * popqueue(buffer_t *buffer) {
    void * element;
    if (empty(buffer)) {
        printf("Buffer underflow\n");
        return "0";
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
 
void * popstack(buffer_t *buffer) {
    int index;
    if (empty(buffer)) {
        printf("Buffer underflow\n");
        return "0";
    } else {
        /* LIFO implementation */
        index = buffer->start + buffer->count - 1;
        if (index >= buffer->size) {
           index = buffer->count - buffer->size - 1;
        }      
        buffer->count--;
        return buffer->element[index];
    }
}
 
int main() {
    buffer_t buffer;
    init(&buffer, 5);
    char *data[] = {"first" , "second", "third", "fourth" , "fifth", "sixth"};
    int i;
    for (i = 0; i < 6; i++) {
        printf("push: %s\n", data[i]);
        push(&buffer, data[i]);
    }
    printf("\n");
    for (i = 0; i < 6; i++) {
        /* need to cast void pointers to char pointers */
        printf("pop from queue: %s\n", (char *)popqueue(&buffer));
    }
     
    for (i = 0; i < 6; i++) {
        printf("push: %s\n", data[i]);
        push(&buffer, data[i]);
    }
    for (i = 0; i < 6; i++) {
        printf("pop from stack: %s\n", (char *)popstack(&buffer));
    }
     
     
}