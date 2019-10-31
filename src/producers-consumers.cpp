/**
        C++ Producer Consumer using C++11 thread facilities
        To compile: g++ -std=c++11 <program name> -pthread -lpthread -o pc
*/
#include <iostream>
#include <sstream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <chrono>
#include <bits/stdc++.h>
using namespace std;

// print function for "thread safe" printing using a stringstream
// void print(ostream &s)
// {
//     cout << s.rdbuf();
//     cout.flush();
//     s.clear();
// }

//
//      Constants
//
const int num_producers = 10;             // number of producer thread
const int num_consumers = 10;             // number of consumer thread
const int producer_delay_to_produce = 10; // in miliseconds
const int consumer_delay_to_consume = 10; // in miliseconds

const int consumer_max_wait_time = 200; // in miliseconds - max time that a consumer can wait for a product to be produced.

const int max_request = 100; // When producers has produced this quantity they will stop to produce
const int buffer_size = 10;  // Maximum number of products that can be stored in buffers

//
//      Variables
//
atomic<int> num_producers_working(0); // When there's no producer working the consumers will stop, and the program will stop.
queue<int> products;                  // The products queue, here we will store our products
mutex xmutex;                         // Our mutex, without this mutex our program will cry

condition_variable is_not_full;  // to indicate that our queue is not full between the thread operations
condition_variable is_not_empty; // to indicate that our queue is not empty between the thread operations

//
//      Functions
//

//      Produce function, producer_id will produce a product
void produce(int producer_id)
{
    unique_lock<mutex> lock(xmutex);
    int product;

    is_not_full.wait(lock, [] { return products.size() != buffer_size; });
    product = products.size();
    products.push(product);

    // print(stringstream() << "Producer " << producer_id << " produced " << product << "\n"); // <-- uncomment to display producer produce product
    is_not_empty.notify_all();
}

//      Consume function, consumer_id will consume a product
void consume(int consumer_id)
{
    unique_lock<mutex> lock(xmutex);
    int product;

    if (is_not_empty.wait_for(lock, chrono::milliseconds(consumer_max_wait_time),
                              [] { return products.size() > 0; }))
    {
        product = products.front();
        products.pop();

        // print(stringstream() << "Consumer " << consumer_id << " consumed " << product << "\n"); // <-- uncomment to display consumers consume product
        is_not_full.notify_all();
    }
}

//      Producer function, this is the body of a producer thread
void producer(int id)
{
    ++num_producers_working;
    for (int i = 0; i < max_request; ++i)
    {
        produce(id);
        this_thread::sleep_for(chrono::milliseconds(producer_delay_to_produce));
    }

    // print(stringstream() << "Producer " << id << " has exited\n"); // <-- uncomment to display end producers thread
    --num_producers_working;
}

//      Consumer function, this is the body of a consumer thread
void consumer(int id)
{
    // Wait until there is any producer working
    while (num_producers_working == 0)
        this_thread::yield();

    while (num_producers_working != 0 || products.size() > 0)
    {
        consume(id);
        this_thread::sleep_for(chrono::milliseconds(consumer_delay_to_consume));
    }

    // print(stringstream() << "Consumer " << id << " has exited\n"); // <-- uncomment to display end consumers thread
}

//
//      Main
//
int main()
{
    // Record Start Time //
    auto start = chrono::high_resolution_clock::now();
    // unsync the I/O of C and C++.
    ios_base::sync_with_stdio(false);

    // Start Producer Consumer Program //
    vector<thread> producers_and_consumers;

    // Create producers
    for (int i = 0; i < num_producers; ++i)
        producers_and_consumers.push_back(thread(producer, i));

    // Create consumers
    for (int i = 0; i < num_consumers; ++i)
        producers_and_consumers.push_back(thread(consumer, i));

    // Wait for consumers and producers to finish
    for (auto &t : producers_and_consumers)
        t.join();

    // End Producer Comsumer Program //

    // Record End time //

    auto end = chrono::high_resolution_clock::now();

    // Calculating total time taken by the program.
    double time_taken =
        chrono::duration_cast<chrono::nanoseconds>(end - start).count();

    time_taken *= 1e-9; // Chang nanosecond to second

    cout << "Time taken by program is : " << fixed
         << time_taken << setprecision(9);
    cout << " seconds." << endl;
    
    return 0;
}