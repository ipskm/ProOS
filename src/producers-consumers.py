#! /usr/bin/env python

import threading
import time
import collections

req = input("Enter value of requsts : ")
bufsz = input("Enter size of buffers : ")
prod = input("Enter the threads of producer : ")
cons = input("Enter the threads of consumer : ")
TO_PRODUCE = int(req)
BUFFER_SIZE = int(bufsz)
PRODUCERS = int(prod)
CONSUMERS = int(cons)

class ProducerConsumer(object):
    def __init__(self, size):
        self.buffer = collections.deque([], size)
        self.mutex = threading.Lock()
        self.notFull = threading.Semaphore(size)
        self.notEmpty = threading.Semaphore(0)

    def append(self, data):
        self.notFull.acquire()
        with self.mutex:
            self.buffer.append(data)
        self.notEmpty.release()

    def remove(self):
        self.notEmpty.acquire()
        with self.mutex:
            data = self.buffer.popleft()
        self.notFull.release()
        return data


def producer(buffer):
    id = threading.current_thread().name
    print("Producer {}".format(id))

    for i in range(TO_PRODUCE):
        data = "{} i: {}".format(id, i)
        buffer.append(data)


def consumer(buffer):
    id = threading.current_thread().name
    print("Consumer {}".format(id))

    for i in range(TO_PRODUCE):
        data = buffer.remove()
        print("{} read: {}".format(id, data))



def main():
    threads = []

    buffer = ProducerConsumer(BUFFER_SIZE)
    for i in range(CONSUMERS):
        c = threading.Thread(target=consumer, args=(buffer,))
        threads.append(c)


    for i in range(PRODUCERS):
        p = threading.Thread(target=producer, args=(buffer,))
        threads.append(p)

    # Start all threads
    for t in threads:
        t.start()

    # Wait for all threads to complete
    for t in threads:
        t.join()


if __name__ == "__main__":
    print("Start Program !")
    _start = time.time()
    main()
    print("Comsume %s requests took %s seconds"% (TO_PRODUCE, (time.time() - _start)))
    print("Throughput %s Successful %s requests/s"% (TO_PRODUCE, (TO_PRODUCE / time.time())))
    print("End Program !")