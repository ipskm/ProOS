/*
    to compile use this command : gcc <filename.cpp> -o <filename>
    software to compile : MinGW-W64 
    link to download : https://sourceforge.net/projects/mingw-w64/
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

// print function for "thread safe" printing ใช้ a stringstream
void print(ostream &s)
{
    cout << s.rdbuf();
    cout.flush();
    s.clear();
}

//
//      Constants
//
const int num_adder = 10;               // จำนวนเทรดของ adder
const int num_remover = 10;             // จำนวนเทรดของ remover
const int delay_for_add = 10;           // in miliseconds
const int delay_for_remove = 10;        // in miliseconds

const int remover_max_wait_time = 200;  // in miliseconds - max time that a remover can wait for a item to be add_itemsd.

const int max_request = 100; // When adders has add_itemsd this quantity they will stop to add_items
const int buffer_size = 10;  // Maximum number of items that can be stored in buffers

//
//      Variables
//
atomic<int> num_removers_working(0);    // When there's no adder working the removers will stop, and the program will stop.
queue<int> items;                       // The items queue, here we will store our items
mutex xmutex;                           // Our mutex, without this mutex our program will cry

condition_variable is_not_full;     // to indicate that our queue is not full between the thread operations
condition_variable is_not_empty;    // to indicate that our queue is not empty between the thread operations

//
//      Functions
//

//      add_items function, adder_id will add_items a item
void add_items(int adder_id)
{
    unique_lock<mutex> lock(xmutex);
    int item;

    is_not_full.wait(lock, [] { return items.size() != buffer_size; });
    item = items.size();
    items.push(item);

    // print(stringstream() << "adder " << adder_id << " add_itemsd " << item << "\n"); // <-- uncomment to display adder add_items item
    is_not_empty.notify_all();
}

//      remove_items function, remover_id will remove_items a item
void remove_items(int remover_id)
{
    unique_lock<mutex> lock(xmutex);
    int item;

    if (is_not_empty.wait_for(lock, chrono::milliseconds(remover_max_wait_time),
                              [] { return items.size() > 0; }))
    {
        item = items.front();
        items.pop();

        // print(stringstream() << "remover " << remover_id << " remove_itemsd " << item << "\n"); // <-- uncomment to display removers remove_items item
        is_not_full.notify_all();
    }
}

//      adder function, this is the body of a adder thread
void adder(int id)
{
    ++num_removers_working;
    for (int i = 0; i < max_request; ++i)
    {
        add_items(id);
        this_thread::sleep_for(chrono::milliseconds(delay_for_add));
    }

    // print(stringstream() << "adder " << id << " has exited\n"); // <-- uncomment to display end adders thread
    --num_removers_working;
}

//      remover function, this is the body of a remover thread
void remover(int id)
{
    // Wait until there is any adder working
    while (num_removers_working == 0)
        this_thread::yield();

    while (num_removers_working != 0 || items.size() > 0)
    {
        remove_items(id);
        this_thread::sleep_for(chrono::milliseconds(delay_for_remove));
    }

    // print(stringstream() << "remover " << id << " has exited\n"); // <-- uncomment to display end removers thread
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

    // Start adder remover Program //
    vector<thread> adders_and_removers;

    // Create adders
    for (int i = 0; i < num_adder; ++i)
        adders_and_removers.push_back(thread(adder, i));

    // Create removers
    for (int i = 0; i < num_remover; ++i)
        adders_and_removers.push_back(thread(remover, i));

    // Wait for removers and adders to finish
    for (auto &t : adders_and_removers)
        t.join();

    // End adder Comsumer Program //

    // Record End time //

    auto end = chrono::high_resolution_clock::now();

    // Calculating total time taken by the program.
    double time_taken =
        chrono::duration_cast<chrono::nanoseconds>(end - start).count();

    time_taken *= 1e-9; // Chang nanosecond to second
    print(stringstream() << "Time taken by program is : " << fixed << time_taken << setprecision(9) << " seconds.\n");
    
    // cout << "Time taken by program is : " << fixed
    //      << time_taken << setprecision(9);
    // cout << " seconds." << endl;
    
    return 0;
}