# งานกลุ่มวิชาระบบปฏิบัติการ ชั้นปีที่ 3 เทอมต้น ปีการศึกษา 2562
***
## การออกเเบบโปรเเกรม
***
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

### การออกเเบบ Append
append_ ฟังก์ชัน รูปร่างหน้าตาของ append_
```c
void append_(int id)
{
    ++num_append_working;
    for (i = 0; i < max_request; i++)
    {
        add_item(id);
        // p_count++;
        this_thread::sleep_for(chrono::nanoseconds(delay_for_add));
    }
    print(stringstream() << "Append Thread id : " << id << " was finished.\n");
    --num_append_working;
}
```
ฟังก์ชัน append จะเพิ่มค่าของ num_append_work แล้วทำการเพิ่มค่าลงไปในบัฟเฟอร์คิว ตามจำนวนของการ รีเควส โดยการให้เทรดที่เรียกใช้งาน append ทำงานในฟังก์ชัน add_item และเมื่อทำงานเสร็จสิ้นจะหยุดการทำงานของเทรดที่เรียก append และ ลดค่าของ num_append_working ลง
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
    print(stringstream() << "Append thread id : " << append_id << " add item " << item << " in buffer.\n");
    is_not_empty.notify_all();
}
```
add_item() ออกแบบให้สามารถเข้าใช้งานได้เพียงเทรดเดียวเท่านั้นโดยค่าที่ จะถูกส่งเข้ามาเป็นค่า thread id ของ append thread จากนั้นจะใช้ mutex ล็อกทรัพยากรบัฟเฟอร์ให้เทรดนั้น ๆ เพื่อไม่ให้เทรดอื่น ๆ ที่รอคอยอยู่เข้ามาทำงานได้ แล้วจะมีการทำงานอยู่ 2 แบบดังนี้
* บัฟเฟอร์เต็ม ก็จะทำการหยุดการทำงานเทรดนั้นชั่วขณะ และส่งสัญญาณว่าคิวไม่ว่างออกไป
* บัฟเฟอร์ไม่เต็ม ก็จะทำการเรียกดูขนาดของคิวมาเก็บไว้ในตัวแปร และ เขียนค่าตัวแปรนั้นลง ไปในคิว และเมื่อทำงานเสร็จก็จะส่งสัญญาณออกไปว่าคิวไม่ว่างแล้ว

***
### การออกเเบบ Remove
remove_ ฟังก์ชัน หน้าตาของ remove
```c
void remove_(int id)
{
    // รอจนกว่าไม่มี append ทำงาน
    while (num_append_working == 0)
        this_thread::yield(); //รอการทำงานซักครู่

    while (num_append_working != 0 || items.size() > 0)
    {
        remove_item(id);                                             
        this_thread::sleep_for(chrono::nanoseconds(delay_for_remove)); //หยุดการทำงานของเทรดเมื่อเสร็จสิ้นโดยจะถูกหยุดไว้เท่ากับเวลาที่กำหนด
    }

    print(stringstream() << "Remove thread id : " << id << " was finished.\n"); // <-- uncomment to display end remove_s thread
}
```
ฟังก์ชัน remove จะตรวจสอบว่าไม่มีเทรด append ทำงานอยู่หรือบัฟเฟอร์ไม่ว่างก็จะเรียกการทำงานของ remove_item เพื่อนำคิวออกจากบัฟเฟอร์จากนั้นก็จะหยุดการทำงานของเทรดนั้นลง

#### ฟังก์ชั่น remove_item
remove_items ฟังก์ชันที่ถูกเรียกใช้งานโดย เทรดต่าง ๆ ของ remove_ เพื่อนำค่าออกจากคิว
```c
void remove_item(int remove_id)
{
    int item;
    unique_lock<mutex> lock(xmutex);
    if (is_not_empty.wait_for(lock, chrono::nanoseconds(max_remove_wait_time),
                              [] { return items.size() > 0; }))
    {
        item = items.front();
        items.pop();         
        c_count++;
        print(stringstream() << "Remove thread id : " << remove_id << " remove " << item << " form buffer.\n");
        is_not_full.notify_all();
    }
}
```
ฟังก์ชัน remove_item จะมีการเข้าถึงฟังก์ชันนี้ได้เพียงแค่เทรดเดียวเท่านั้น เมื่อทำการเรียกใช้งานฟังก์ชั่นก็จะใช้ mutex ล็อกทรัพยาการบัฟเฟอร์ให้เข้าถึงได้เพียงแค่เทรดเดียวและมีการทำงานอยู่ 2 แบบดังนี้
* บัฟเฟอร์ว่าง จะหยุดการทำงานเทรดนั้น ๆ และส่งสัญญาณออกไปให้เทรดอื่น ๆ ที่รอทำงาน
* บัฟเฟอร์ไม่ว่างหรือเต็ม จะไอเท็มที่ฟน้าคิวและทำการ dequeue ออกจากบัฟเฟอร์ และส่งสัญญาณให้เทรดอื่น ๆ ที่รอทำงาน
### Flowchart ของโปรแกรม
![Flowchart](./image/OS.png)
***
## เงื่อนไข วิธีการทำงาน เเละการพิสูจน์คุณสมบัติของ Append
### ฟังก์ชั่น Append
```c
void append_(int id)
{
    ++num_append_working;
    for (i = 0; i < max_request; i++)
    {
        add_item(id);
        // p_count++;
        this_thread::sleep_for(chrono::nanoseconds(delay_for_add));
    }
    print(stringstream() << "Append Thread id : " << id << " was finished.\n");
    --num_append_working;
}
```
#### เงื่อนไขของ Append
#### วิธีการทำงานของ Append
#### การพิสูจน์คุณสมบัติของ Append
***
## เงื่อนไข เเละวิธีการทำงานของ Remove
#### เงื่อนไขของ Remove
#### วิธีการทำงานของ Remove
***
## ผลการ Run & Result
***
## Source code 
***
