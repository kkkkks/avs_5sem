#include <iostream>
#include <chrono>
#include <ctime>
#include <mutex>
#include <atomic>
#include <thread>
#include <queue>
#include <thread>

using namespace std;

atomic <int> atomic_counter(0);
const int TaskNum =  1024;
uint8_t Tasks[TaskNum];
const int QueueSize[3] = { 1, 4, 16 };

int total = 0;

int increment() {
    return atomic_counter++;
}

// динамическая очередь
class queue1 
{
private:
    mutex mutex;
    queue <uint8_t> q;
public:
    void push(uint8_t val) {
        mutex.lock();
        q.push(val);
        mutex.unlock();
    }
    bool pop(uint8_t& val) {
        mutex.lock(); 
        if (q.empty() == true) { 
            mutex.unlock();
            this_thread::sleep_for(chrono::milliseconds(1)); 
            mutex.lock();
            if (q.empty() == true) {  
                mutex.unlock();
                return false;
            }
        }
        val = q.front(); 
        q.pop();
        mutex.unlock(); 
        return true;
    }
    bool isempty() {
        return q.size();
    }
};

// очередь фикс.размера
class queue2
{
private:
    uint8_t QueueSize = 1;
    mutex mutex1;
    condition_variable cv;
    queue <uint8_t> q;
    bool ready = true;
public:
    void setSize(int size) {
        QueueSize = size;
    }
    void push(uint8_t val) {
        unique_lock<mutex> ulock(mutex1);
        while (q.size() >= QueueSize) {
            cv.wait(ulock); 
        }
        q.push(val); 
        ulock.unlock();
        cv.notify_one();
    }
    bool pop(uint8_t& val) {
        unique_lock<mutex> ulock(mutex1);
        if (q.empty() == true) {
            cv.wait_for(ulock, chrono::milliseconds(1));
            if (q.empty() == true) {
                ulock.unlock();
                return false;
            }
        }
        val = q.front();
        q.pop();
        ulock.unlock();
        cv.notify_one();
        return true;
    }
};

queue2 q2;
queue1 q1;

void producer_func(bool queue) {
    clock_t start = clock();
    for (int i = 0; i < TaskNum; i++) {
        if (queue) {
            q2.push(Tasks[i]); // записываем TaskNum едниниц в очередь
        }
        else {
            q1.push(Tasks[i]);
        }
    }
    clock_t end = clock();
    int time = (end - start) / (CLOCKS_PER_SEC / 1000);
    printf("Time in producer thread: %d ms\n", time);
}

void consumer_func(int ProducerNum, bool queue)
{
    clock_t start = clock();
    uint8_t num;
    int temp = 0;
    int value = 0;
    while (temp < TaskNum * ProducerNum) {
        temp = atomic_counter.load();
        if (queue) {
            if (q2.pop(num)) { // достаем значения из очереди
                increment();
                value++;
            }
        }
        else {
            if (q1.pop(num)) {
                increment();
                value++;
            }
        }
    }
    clock_t end = clock();
    total += value;
    int time = (end - start) / (CLOCKS_PER_SEC / 1000);
    printf("Time in consumer thread: %d ms\n", time);
}

void task(bool task) {
    if (task)
        cout << "Task 2: fixed size queue\n\n";
    else
        cout << "Task 1: dynamic queue\n\n";

    const int ConsProdNum[3] = { 1, 2, 4 };
    for (int i = 0; i < 3; i++) {
        cout << "Size of consumers/producers: " << ConsProdNum[i] << endl;    
        total = 0;   
        thread* consumer = new thread[ConsProdNum[i]];
        thread* producer = new thread[ConsProdNum[i]];
        for (int k = 0; k < ConsProdNum[i]; k++) {
             producer[k] = thread(producer_func, task);
             consumer[k] = thread(consumer_func, ConsProdNum[i], task);
        }
        for (int k = 0; k < ConsProdNum[i]; k++) {
             producer[k].join();
             consumer[k].join();
        }
        cout << "Total sum: " << total / ConsProdNum[i];
        if (total == TaskNum * ConsProdNum[i]) {
            cout << "\nCORRECT\n\n";
        }
        else cout << "\nNOT CORRECT\n\n";
        atomic_counter.store(0);
         delete[] consumer, producer;
    }
}

int main()
{
    task(false);
    cout << "--------------------------------\n";
    for (int i = 0; i < 3; i++) {
        q2.setSize(QueueSize[i]);
        cout << "Queue size: " << QueueSize[i] << endl;
        task(true);
    }
}