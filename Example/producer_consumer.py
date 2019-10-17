"""
Solving producer-consumer problem using semaphores
"""

import threading


# Buffer size
N = 10
# Buffer init
buf = [0] * N


fill_count = threading.Semaphore(0)
empty_count = threading.Semaphore(N)


def produce():
    print("One item produced!")
    return 1


def producer():
    front = 0
    while True:
        x = produce()
        empty_count.acquire()
        buf[front] = x
        fill_count.release()
        front = (front + 1) % N


def consume(y):
    print("One item consumed!")


def consumer():
    rear = 0
    while True:
        fill_count.acquire()
        y = buf[rear]
        empty_count.release()
        consume(y)
        rear = (rear + 1) % N


producer_thread = threading.Thread(target=producer)
consumer_thread = threading.Thread(target=consumer)

producer_thread.start()
consumer_thread.start()