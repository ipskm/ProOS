import multiprocessing as mp  
import time

def worker(queue, kafka_queue):  
    while True:
        msg = queue.get()         # Read from the queue and do nothing
       # print('Processing %s (MP: %s) ' % (msg, mp.current_process().name))
        time.sleep(0.01)
        kafka_queue.put(msg)
        queue.task_done()

def send_kafka(queue):  
    while True:
        msg = queue.get()
        #print('Kafka sending %s' % msg)
        queue.task_done()


def writer(count, queue):  
    ## Write to the queue
    for ii in range(0, count):
       # print('Writing %s (MP: %s)' % (ii, mp.current_process().name))
        queue.put(ii)             # Write 'count' numbers into the queue
        time.sleep(0.001)

if __name__=='__main__':  
    bufsize = input("Enter the size of buffer : ")
    count = int(bufsize)
    prod = input('Enter the numbers of comsumer : ')
    number = int(prod)
    # initialize queues
    queue = mp.JoinableQueue()   # this is where we are going to store input data
    kafka_queue = mp.JoinableQueue()  # this where we are gonna push them out

    # create 4 processes, which takes queues as arguments
    # the data will get in and out through the queues
    # daemonize it
    processes = []
    for i in range(number):
        worker_process = mp.Process(target=worker, args=(queue, kafka_queue), daemon=True, name='worker_process_{}'.format(i))
        worker_process.start()        # Launch reader() as a separate python process
        processes.append(worker_process)

   # print([x.name for x in processes])

    # this process simulates sending processed data out 
    kafka_process = mp.Process(target=send_kafka, args=(kafka_queue,), daemon=True, name='kafka_process')
    kafka_process.start()


    _start = time.time()
    writer(count, queue)    # Send a lot of stuff for workers tasks
    # wait till everything is processed
    queue.join()
    kafka_queue.join()
    print( "Sending %s numbers to Queue() took %s seconds" % (count, (time.time() - _start)))