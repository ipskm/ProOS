from Queue import Queue
from threading import Thread
from random import randrange

queue = Queue(10)

class Consumer(Thread):
    def __init__(self, queue):
        Thread.__init__(self)
        self.queue = queue

    def run(self):
        while True:
            num = self.queue.get()
            self.queue.task_done()
            print 'consumer get number: ' + str(num)



class Producer(Thread):
    def __init__(self, queue):
        Thread.__init__(self)
        self.queue = queue

    def run(self):
        while True:
            num = randrange(1, 10)
            self.queue.put(num)

            print 'producer produce number : ' + str(num)


def main():
    for i in range(5):
        p = Producer(queue)
        c = Consumer(queue)
        p.setDaemon(True)
        c.setDaemon(True)
        p.start()
        c.start()

    queue.join()


if __name__ == "__main__":
    main()