/*
    คอมไพล์ไฟล์นี้โดยใช้คำสั่ง : g++ ชื่อไฟล์.cpp -o ชื่อโปรแกรมที่ต้องการ
    รันโปรแกรม : ./ชื่อโปรแกรม
    คอมไพล์เลอร์ : MinGW-W64 
    ลิงก์ดาวน์โหลด : https://sourceforge.net/projects/mingw-w64/
    ติดตั้งและเซตพาร์ธให้เรียบร้อย
*/
//นำเข้าไลบราลี่ที่จำเป็น
#include <iostream>         //อินพุทเอาท์พุททั่วไป
#include <sstream>          //รวมข้อควม
#include <vector>           //เวกเตอร์
#include <queue>            //คิว
#include <thread>           //เทรด
#include <mutex>            //มิวเทกซ์
#include <atomic>           //อะตอมิก
#include <condition_variable>   //ตัวแปรคงที่
#include <chrono>           //โครโน
#include <bits/stdc++.h>    //ไลบราลี่ทั่วไป
using namespace std;  

// print ฟังก์ชั่น สำหรับการแสดงผลโดยใช้ stringstream ฟังก์ชัน
void print(ostream &s)
{
    cout << s.rdbuf();
    cout.flush();
    s.clear();
}

//
//      ค่าคงที่
//
const int num_append_ = 10;               // จำนวนเทรดของ append_
const int num_remove = 10;             // จำนวนเทรดของ remove_
const int delay_for_add = 10;           // เวลาสำหรับการเพิ่มข้อมูลลงคิว
const int delay_for_remove = 10;        // เวลาสำหรับการดึงข้อมูลออกจากคิว

const int max_remove_wait_time = 200;  // เวลาที่มากทีสุดที่การเรียกใช้ remove รอ append
const int max_request = 100; // เมื่อถึงค่าที่กำหนด add_items จะหยุดการทำงาน
const int buffer_size = 10;  // ค่าสูงสุดที่คิวสามารถบรรจุได้

//
//      Variables
//
atomic<int> num_append_working(0);      // เมื่อไม่มี append ทำงาน remove ทำงานเสร็จ ก็จะหยุดโปรแกรม
queue<int> items;                       // ไอเท็มในคิว
mutex xmutex;                           // มิวเทกซ์ที่ใช้จัดการการเข้าถึงคิวของเธรดต่าง ๆ เพื่อป้องกันการเรียกใช้งานแบบซ้ำซ้อน

condition_variable is_not_full;     // ตรวจสอบว่าคิวไม่เต็มในระหว่างการทำงานของเทรด
condition_variable is_not_empty;    // ตรวจสอบว่าคิวไม่ว่างในระหว่างการทำงานของเทรด

//
//      ฟังก์ชัน
//

//      add_item ฟังก์ชัน ที่ถูกเรียกใช้งานโดย เทรดต่าง ๆ ของ append_ เพื่อนำค่าเข้าไปในคิว
void add_items(int append__id)
{
    unique_lock<mutex> lock(xmutex);    //ล็อคมิวเทคเพื่อทำงาน
    int item;

    is_not_full.wait(lock, [] { return items.size() != buffer_size; }); //ตรวจสอบว่าคิวไม่ได้เต็ม
    item = items.size(); //เรียกขนาดของคิวปัจจุบัน
    items.push(item); //เพิ่มค่าลงในคิว

    // print(stringstream() << "append_ " << append__id << " add_itemsd " << item << "\n"); // <-- เลิกคอมเม้นเพื่อดูผล

//      remove_items ฟังก์ชันที่ถูกเรียกใช้งานโดย เทรดต่าง ๆ ของ remove_ เพื่อนำค่าออกจากคิว
void remove_items(int remove_id)
{
    unique_lock<mutex> lock(xmutex);
    int item;

    if (is_not_empty.wait_for(lock, chrono::milliseconds(max_remove_wait_time),
                              [] { return items.size() > 0; }))
    {
        item = items.front();
        items.pop();

        // print(stringstream() << "remove_ " << remove_id << " remove_itemsd " << item << "\n"); // <-- เลิกคอมเม้นเพื่อดูผล
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

    // Create append_ thread
    for (int i = 0; i < num_append_; ++i)
        append_s_and_remove_s.push_back(thread(append_, i));

    // Create remove_ thread
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