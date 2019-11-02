/*
    คอมไพล์ไฟล์นี้โดยใช้คำสั่ง : g++ ชื่อไฟล์.cpp -pthread -lpthread -o ชื่อโปรแกรมที่ต้องการ
    รันโปรแกรม : ./ชื่อโปรแกรม
    คอมไพล์เลอร์ : MinGW-W64 
    ลิงก์ดาวน์โหลด : https://sourceforge.net/projects/mingw-w64/
    ติดตั้งและเซตพาร์ธให้เรียบร้อย
*/

#include <iostream>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <bits/stdc++.h>
#include <boost/circular_buffer.hpp>

using namespace std;

#define BUFFER_SIZE 10
#define REQUEST 10

double c_count = 0;

condition_variable is_not_full;
condition_variable is_not_empty;
mutex xmutex;

boost::circular_buffer<int> myringbuf(BUFFER_SIZE);

void add_item(){
    int random_num = rand()%100;
    for(int i; i<REQUEST;i++){
        myringbuf.push_back(random_num);
    }
}

int main(){
    

}