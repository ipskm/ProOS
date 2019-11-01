#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

//define variable
#define BUFFER_SIZE 5
#define CONS_NUM
#define PROD_NUM
#define REQEST 10

#define ERROR_EMPTY 0
#define ERROR_FULL 0xFF
int head = 0, tail = 0;
int count = 0;
char buffer[BUFFER_SIZE];
// reads a byte from the buffer and return ERROR_EMPTY if buffer empty
char fifoRead() {
   if (0 == count) return ERROR_EMPTY;
   count --;
   tail = (tail + 1) % BUFFER_SIZE;
   return buffer[tail];
}
 
// writes a byte to the buffer if not ERROR_FULL
char fifoWrite(char val) {
   if (BUFFER_SIZE == count) return ERROR_FULL;
   count ++;
   head = (head + 1) % BUFFER_SIZE;
   return buffer[head];
}

int main(){
    fifoWrite('A');
    char read = fifoRead();
    printf("%s",&read);
}