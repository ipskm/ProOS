# งานกลุ่มวิชาระบบปฏิบัติการ ชั้นปีที่ 3 เทอมต้น ปีการศึกษา 2562
---
## การออกเเบบโปรเเกรม
---
### การออกเเบบ Buffer
```C
#include <queue>
queue<int> queue_name;
```
#### คิวในไลบรารีแม่แบบมาตรฐาน (STL)
Queues เป็นประเภทของอะแดปเตอร์ตู้คอนเทนเนอร์ที่ทำงานในการจัดเรียงแบบ first out (FIFO) องค์ประกอบถูกแทรกที่ด้านหลัง (สิ้นสุด) และถูกลบออกจากด้านหน้า  
ฟังก์ชั่นที่รองรับโดยคิวคือ:  
1. empty () - ส่งคืนว่าคิวว่างเปล่าหรือไม่
2. size () - ส่งคืนขนาดของคิว
3. queue::swap () ใน C ++ STL: แลกเปลี่ยนเนื้อหาของสองคิว แต่คิวจะต้องเป็นประเภทเดียวกันแม้ว่าขนาดอาจแตกต่างกัน
4. queue::emplace () ใน C ++ STL: แทรกองค์ประกอบใหม่ลงในคอนเทนเนอร์คิวองค์ประกอบใหม่จะถูกเพิ่มลงในตอนท้ายของคิว
5. queue::front() และ queue::back () ใน C ++ STL – front() ฟังก์ชันส่งคืนการอ้างอิงไปยังองค์ประกอบแรกของคิว back () ฟังก์ชันส่งคืนการอ้างอิงไปยังองค์ประกอบสุดท้ายของคิว
6. push(g) และ pop() - ฟังก์ชั่น push() เพิ่มองค์ประกอบ ‘g’ ที่ส่วนท้ายของคิว ฟังก์ชั่น pop () ลบองค์ประกอบแรกของคิว
---
### การออกเเบบ Append
append_ ฟังก์ชัน รูปร่างหน้าตาของ append_
```c
void append_(int id)
{
    ++num_append_working; //บวกค่าของ append_working แล้วทำการเพิ่มคิวลงข้อมูล
    for (i = 0; i < max_request; i++)
    {
        add_item(id);
        // p_count++;
        this_thread::sleep_for(chrono::nanoseconds(delay_for_add)); //เมื่อเพิ่มเสร็จสลับคิวไปให้ตัวใหม่ได้ทำ
    }
    print(stringstream() << "Append Thread id : " << id << " was finished.\n"); // <-- uncomment to display end append_s thread
    --num_append_working; //ลบค่าของ append_working ออก
}
```
ฟังก์ชั่น Append จะบวกค่าของ append_working เเละวนลูปเพื่อส่งค่า id ตาม max_request ไปยังฟังก์ชั่น add_item เมื่อเสร็จเเล้วจะทำ
#### ฟังก์ชั่น add_item
 add_item ฟังก์ชัน ที่ถูกเรียกใช้งานโดย เทรดต่าง ๆ ของ append_ เพื่อนำค่าเข้าไปในคิว
```C
void add_item(int append_id)
{

    int item;
    unique_lock<mutex> lock(xmutex);                                   
    is_not_full.wait(lock, [] { return items.size() != buffer_size; });
    item = items.size();                                               

    items.push(item);
    p_count++;
    // print(stringstream() << "Append thread id : " << append_id << " add item " << item << " in buffer.\n"); // <-- เลิกคอมเม้นเพื่อดูผล
    is_not_empty.notify_all();
}
```
---
