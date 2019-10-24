#include <iostream>
#include <thread>
#include <array>
#include <vector>
#include <mutex>
#include <string>
#include <condition_variable>
#include <queue>
#include <algorithm>
 
using namespace std;
 
mutex m;
condition_variable producer_cv, consumer_cv;
queue<int> buffer;
 
const int BUFFER_SIZE = 10;
 
void consumer_thread(int id)
{
    while (true)
    {
        unique_lock<mutex> lk(m);
 
        if (!buffer.empty())
        {           
            cout << "Consumer " << id << " ate " << buffer.front() << endl;
            buffer.pop();           
            producer_cv.notify_all();                   
        }
        else
        {
            cout << "*** Consumer " << id << " is waiting" << endl;
            consumer_cv.wait(lk, []{ return !buffer.empty(); });
        }
     
        lk.unlock();
        this_thread::sleep_for(chrono::milliseconds(random() % 400 + 800));
    }
}
 
void producer_thread(int id)
{
    while (true)
    {
        unique_lock<mutex> lk(m);
 
        if (buffer.size() < BUFFER_SIZE)
        {
            int a = random() % 400; 
            cout << "Producer " << id << " produced: " << a << endl;
 
            buffer.push(a);
            consumer_cv.notify_all();               
        }
        else
        {
            cout << "### Producer " << id << " is waiting" << endl;
            producer_cv.wait(lk, []{ return buffer.size() < BUFFER_SIZE; });
        }
             
        lk.unlock();
        this_thread::sleep_for(chrono::milliseconds(random() % 400 + 100));
    }
}
 
int main()
{
    const int consumers_count = 5;
    const int producers_count = 3;
 
    vector<thread> producers;
    vector<thread> consumers;
 
    for (int i = 0; i < consumers_count; i++)
        consumers.push_back(thread(consumer_thread, i + 1));
    for (int i = 0; i < producers_count; i++)
        producers.push_back(thread(producer_thread, i + 1));
 
    for (int i = 0; i < consumers_count; i++)
        consumers[i].join();
    for (int i = 0; i < producers_count; i++)
        producers[i].join();
 
    system("pause");
    return 0;
}
