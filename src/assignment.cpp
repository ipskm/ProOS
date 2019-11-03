/*
    คอมไพล์ไฟล์นี้โดยใช้คำสั่ง : g++ ชื่อไฟล์.cpp -pthread -lpthread -o ชื่อโปรแกรมที่ต้องการ
    รันโปรแกรม : ./ชื่อโปรแกรม
    คอมไพล์เลอร์ : MinGW-W64 
    ลิงก์ดาวน์โหลด : https://sourceforge.net/projects/mingw-w64/
    ติดตั้งและเซตพาร์ธให้เรียบร้อย
    ไลบราลี่ที่จำเป็น : Boost C++ Libraly
    ลิงก์ดาวน์โหลด : https://www.boost.org/
*/

#include <iostream>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <atomic>
#include <vector>
#include <chrono>
#include <bits/stdc++.h>
#include <boost/circular_buffer.hpp>

//or using this include if you don't install boost libraly//
// #include "circular_buffer.hpp" //<- uncomment this line


using namespace std;

#define BUFFER_SIZE 10
#define REQUEST 1000000
#define PROD 20
#define CONS 30

double c_count = 0;
double c_remove = 0;
int i = 0;
const int wait_time = 1;
atomic<int> num_append_working(0);
condition_variable is_not_full;
condition_variable is_not_empty;
mutex xmutex, ymutex;

boost::circular_buffer<int> myringbuf(BUFFER_SIZE);

void add_item(int append_id)
{
    int random_num = rand() % 100;
    unique_lock<mutex> lock(ymutex);
    is_not_full.wait(lock, [] { return myringbuf.size() != BUFFER_SIZE; });
    myringbuf.push_back(random_num);
    // cout << "Append ID : " << append_id << " add " << random_num << endl;
    is_not_empty.notify_all();
}

void remove_item(int remove_id)
{
    unique_lock<mutex> lock(ymutex);
    int product;
    if (is_not_empty.wait_for(lock, chrono::nanoseconds(wait_time),
                              [] { return myringbuf.size() > 0; }))
    {
        product = myringbuf.front();
        myringbuf.pop_front();
        ++c_count;
        // cout << "Remove ID : " << remove_id << " remove " << product << endl;
        is_not_full.notify_all();
    }
}

void append_(int id)
{
    ++num_append_working;
    while (i < REQUEST)
    {
        add_item(id);
        // this_thread::sleep_for(chrono::nanoseconds(wait_time));
        ++i;
    }
    this_thread::sleep_for(chrono::nanoseconds(wait_time));
    --num_append_working;
}

void remove_(int id)
{
    while (num_append_working == 0)
    {
        this_thread::yield();
    }
    while (num_append_working != 0 || myringbuf.size() > 0)
    {
        // xmutex.lock();
        remove_item(id);
        this_thread::sleep_for(chrono::nanoseconds(wait_time));
        // xmutex.unlock();
    }
}
int main()
{
    // Record Start Time //
    auto start = chrono::high_resolution_clock::now();
    // unsync the I/O of C and C++.
    ios_base::sync_with_stdio(false);

    // Start Producer Consumer Program //
    vector<thread> producers_and_consumers;

    vector<thread> vec_of_thread;
    for (int i = 0; i < PROD; i++)
    {
        vec_of_thread.push_back(thread(append_, i));
    }
    for (int i = 0; i < CONS; i++)
    {
        vec_of_thread.push_back(thread(remove_, i));
    }
    for (auto &t : vec_of_thread)
    {
        t.join();
    }

    cout << "CONSUME1 : " << c_count << " COMSUME2 : " << c_remove << endl;

    auto end = chrono::high_resolution_clock::now();

    // Calculating total time taken by the program.
    double time_taken =
        chrono::duration_cast<chrono::nanoseconds>(end - start).count();

    time_taken *= 1e-9; // Chang nanosecond to second
    double through_put = (double)c_count / time_taken;
    double percentage = (c_count / REQUEST) * 100.0;
    cout << "Producer " << PROD
         << " Comsumer " << CONS
         << "\nBuffer Size " << BUFFER_SIZE
         << "\nRequest " << REQUEST
         << endl;
    cout << "Success fully consume " << c_count
         << " requests. (" << percentage
         << "%)" << endl;
    cout << "Elapsed Time : " << fixed << time_taken
         << setprecision(9) << " Seconds." << endl;
    cout << "Throughput " << through_put << " request/s" << endl;
}