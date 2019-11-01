/*
    to compile use this command : g++ <filename.cpp> -o <filename>
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
const int num_append_ = 10;               // จำนวนเทรดของ append_
const int num_remove = 10;             // จำนวนเทรดของ remove_
const int delay_for_add = 10;           // in miliseconds
const int delay_for_remove = 10;        // in miliseconds

const int max_remove_wait_time = 200;  // in miliseconds - max time that a remove_ can wait for a item to be add_itemsd.

const int max_request = 100; // When append_s has add_itemsd this quantity they will stop to add_items
const int buffer_size = 10;  // Maximum number of items that can be stored in buffers

//
//      Variables
//
atomic<int> num_append_working(0);    // When there's no append_ working the remove_s will stop, and the program will stop.
queue<int> items;                       // The items queue, here we will store our items
mutex xmutex;                           // Our mutex, without this mutex our program will cry

condition_variable is_not_full;     // to indicate that our queue is not full between the thread operations
condition_variable is_not_empty;    // to indicate that our queue is not empty between the thread operations

//
//      Functions
//

//      add_items function, append__id will add_items a item
void add_items(int append__id)
{
    unique_lock<mutex> lock(xmutex);
    int item;

    is_not_full.wait(lock, [] { return items.size() != buffer_size; });
    item = items.size();
    items.push(item);

    // print(stringstream() << "append_ " << append__id << " add_itemsd " << item << "\n"); // <-- uncomment to display append_ add_items item
    is_not_empty.notify_all();
}

//      remove_items function, remove_id will remove_items a item
void remove_items(int remove_id)
{
    unique_lock<mutex> lock(xmutex);
    int item;

    if (is_not_empty.wait_for(lock, chrono::milliseconds(max_remove_wait_time),
                              [] { return items.size() > 0; }))
    {
        item = items.front();
        items.pop();

        // print(stringstream() << "remove_ " << remove_id << " remove_itemsd " << item << "\n"); // <-- uncomment to display remove_s remove_items item
        is_not_full.notify_all();
    }
}

//      append_ function, this is the body of a append_ thread
void append_(int id)
{
    ++num_append_working;
    for (int i = 0; i < max_request; ++i)
    {
        add_items(id);
        this_thread::sleep_for(chrono::milliseconds(delay_for_add));
    }

    // print(stringstream() << "append_ " << id << " has exited\n"); // <-- uncomment to display end append_s thread
    --num_append_working;
}

//      remove_ function, this is the body of a remove_ thread
void remove_(int id)
{
    // Wait until there is any append_ working
    while (num_append_working == 0)
        this_thread::yield();

    while (num_append_working != 0 || items.size() > 0)
    {
        remove_items(id);
        this_thread::sleep_for(chrono::milliseconds(delay_for_remove));
    }

    // print(stringstream() << "remove_ " << id << " has exited\n"); // <-- uncomment to display end remove_s thread
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

    // Start append_ remove_ Program //
    vector<thread> append_s_and_remove_s;

    // Create append_s
    for (int i = 0; i < num_append_; ++i)
        append_s_and_remove_s.push_back(thread(append_, i));

    // Create remove_s
    for (int i = 0; i < num_remove; ++i)
        append_s_and_remove_s.push_back(thread(remove_, i));

    // Wait for remove_s and append_s to finish
    for (auto &t : append_s_and_remove_s)
        t.join();

    // End append_ Comsumer Program //

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