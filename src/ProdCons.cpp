/*
    คอมไพล์ไฟล์นี้โดยใช้คำสั่ง : g++ ชื่อไฟล์.cpp -pthread -lpthread -o ชื่อโปรแกรมที่ต้องการ
    รันโปรแกรม : ./ชื่อโปรแกรม
    คอมไพล์เลอร์ : MinGW-W64 
    ลิงก์ดาวน์โหลด : https://sourceforge.net/projects/mingw-w64/
    ติดตั้งและเซตพาร์ธให้เรียบร้อย
*/
//นำเข้าไลบราลี่ที่จำเป็น
#include <iostream>           //อินพุทเอาท์พุททั่วไป
#include <sstream>            //รวมข้อควม
#include <vector>             //เวกเตอร์
#include <queue>              //คิว
#include <thread>             //เทรด
#include <mutex>              //มิวเทกซ์
#include <atomic>             //อะตอมิก
#include <condition_variable> //ตัวแปรคงที่
#include <chrono>             //โครโน
#include <bits/stdc++.h>      //ไลบราลี่ทั่วไป
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
const int num_append_ = 200;     // จำนวนเทรดของ append_
const int num_remove_ = 300;     // จำนวนเทรดของ remove_
const int delay_for_add = 500;    // เวลาสูงสุดสำหรับการเพิ่มข้อมูลลงคิว ในหน่วย นาโนวินาที
const int delay_for_remove = 500; // เวลาสูงสุดสำหรับการดึงข้อมูลออกจากคิว ในหน่วย นาโนวินาที

const int max_remove_wait_time = 100; // เวลาที่มากทีสุดที่การเรียกใช้ remove รอ append
const int max_request = 1000000000;       // เมื่อถึงค่าที่กำหนด add_items จะหยุดการทำงาน
const int buffer_size = 1000;         // ค่าสูงสุดที่คิวสามารถบรรจุได้
double c_count = 0;                   //นับค่าการ comsume
double p_count = 0;                   //นับค่าการผลิต
int i;
//
//      Variables
//
atomic<int> num_append_working(0); // เมื่อไม่มี append ทำงาน remove ทำงานเสร็จ ก็จะหยุดโปรแกรม
queue<int> items;                  // สร้างคิวที่ชื่อ items
mutex xmutex;                      // มิวเทกซ์ที่ใช้จัดการการเข้าถึงคิวของเธรดต่าง ๆ เพื่อป้องกันการเรียกใช้งานแบบซ้ำซ้อน

condition_variable is_not_full;  // ตรวจสอบว่าคิวไม่เต็มในระหว่างการทำงานของเทรด
condition_variable is_not_empty; // ตรวจสอบว่าคิวไม่ว่างในระหว่างการทำงานของเทรด

//
//      ฟังก์ชัน
//

//      add_item ฟังก์ชัน ที่ถูกเรียกใช้งานโดย เทรดต่าง ๆ ของ append_ เพื่อนำค่าเข้าไปในคิว
void add_item(int append_id)
{

    int item;
    unique_lock<mutex> lock(xmutex);                                    //ล็อคมิวเทคเพื่อทำงาน
    is_not_full.wait(lock, [] { return items.size() != buffer_size; }); //ตรวจสอบว่าคิวไม่ได้เต็ม
    item = items.size();                                                //เรียกขนาดของคิวปัจจุบัน

    items.push(item); //เพิ่มค่าลงในคิว
    p_count++;
    // print(stringstream() << "Append thread id : " << append_id << " add item " << item << " in buffer.\n"); // <-- เลิกคอมเม้นเพื่อดูผล
    is_not_empty.notify_all();
}
/*      remove_items ฟังก์ชันที่ถูกเรียกใช้งานโดย เทรดต่าง ๆ ของ remove_ เพื่อนำค่าออกจากคิว */
void remove_item(int remove_id)
{
    int item;
    unique_lock<mutex> lock(xmutex);
    //ตรวจสอบว่าคิวนั้นไม่ว่างเปล่า
    if (is_not_empty.wait_for(lock, chrono::nanoseconds(max_remove_wait_time),
                              [] { return items.size() > 0; }))
    {
        item = items.front(); //เลือกไอเทมที่เว็กเตอร์ชี้อยู่
        items.pop();          //ลบไอเทมออกจากคิว
        c_count++;
        // print(stringstream() << "Remove thread id : " << remove_id << " remove " << item << " form buffer.\n"); // <-- เลิกคอมเม้นเพื่อดูผล
        is_not_full.notify_all(); //เมื่อคิวไม่เต็มแล้วส่งสัญญาณออกไป
    }
}

//      append_ ฟังก์ชัน รูปร่างหน้าตาของ append_
void append_(int id)
{
    ++num_append_working; //บวกค่าของ append_working แล้วทำการเพิ่มคิวลงข้อมูล
    for (i = 0; i < max_request; i++)
    {
        add_item(id);
        // p_count++;
        this_thread::sleep_for(chrono::milliseconds(delay_for_add)); //เมื่อเพิ่มเสร็จสลับคิวไปให้ตัวใหม่ได้ทำ
    }
    // print(stringstream() << "Append Thread id : " << id << " was finished.\n"); // <-- uncomment to display end append_s thread
    --num_append_working; //ลบค่าของ append_working ออก
}

//      remove_ ฟังก์ชัน หน้าตาของ remove
void remove_(int id)
{
    // รอจนกว่าไม่มี append ทำงาน
    while (num_append_working == 0)
        this_thread::yield(); //รอการทำงานซักครู่

    while (num_append_working != 0 || items.size() > 0)
    {
        remove_item(id);
        // c_count++;                                               //ลบไอเทมออก
        this_thread::sleep_for(chrono::milliseconds(delay_for_remove)); //ปิดตัวเองลงไปให้ตัวอื่นมาทำต่อ
    }

    // print(stringstream() << "Remove thread id : " << id << " was finished.\n"); // <-- uncomment to display end remove_s thread
}

//
//      Main
//
int main()
{
    //เริ่มบันทึกเวลาที่โปรแกรมรัน
    auto start = chrono::high_resolution_clock::now();
    // ไม่นับค่าเวลาของ i/o
    ios_base::sync_with_stdio(true);

    // เริ่มการทำงานโปรแกรม
    vector<thread> append_s_and_remove_s;
    // สร้างเทรด append
    for (int i = 0; i < num_append_; ++i)
        append_s_and_remove_s.push_back(thread(append_, i));

    // สร้างเทรด remove
    for (int i = 0; i < num_remove_; ++i)
        append_s_and_remove_s.push_back(thread(remove_, i));

    // รอจนกว่าสร้างเสร็จแล้วให้เริ่มการทำงาน
    for (auto &t : append_s_and_remove_s)
        t.join();

    // จบโปรแกรม

    // บันทึกค่าเวลาที่ทำงานเสร็จ

    auto end = chrono::high_resolution_clock::now();

    // คำนวณเวลาทั้งหมด
    double time_taken =
        chrono::duration_cast<chrono::nanoseconds>(end - start).count();

    time_taken *= 1e-9; // เปลี่ยนค่าจากนาโนวินาทีเป็นวินาที
                        //แสดงผลค่าเวลา
    double through_put = (double)c_count / time_taken;
    double percentage = (c_count / max_request) * 100.0;
    print(stringstream() << "Producer " << num_append_ << " Consumer " << num_remove_ << "\nBuffer size " << buffer_size << "\nRequest " << max_request << "\n");
    print(stringstream() << "Success fully consume " << c_count << " request (" << percentage << "%)\n"); //แสดงผลการนับ
    print(stringstream() << "Elapsed Time : " << fixed << time_taken << setprecision(9) << " seconds.\n");
    print(stringstream() << "Throughput " << through_put << "request/s");
    return 0;
}