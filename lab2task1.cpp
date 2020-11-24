#include <iostream>
#include <chrono>
#include <ctime>
#include <mutex>
#include <atomic>
#include <thread>
#include <vector>

using namespace std;

mutex Mutex;
unsigned int counter_mutex = 0;
atomic <int> counter_atomic(0);

int TaskNum = 1024 * 1024;
vector<unsigned char> bytes(TaskNum, 0);

int increment() {
    return counter_atomic++;
}

void thread_action(int time_sleep, int counter_type) {
    clock_t start = clock();
    int temp;
    while (true) {
        if (counter_type == 1) {
            Mutex.lock();
            temp = counter_mutex;
            counter_mutex++;
            Mutex.unlock();
        }
        else if (counter_type == 2) {
            temp = increment();
        }
        if (temp < TaskNum) {
            bytes[temp] += 1;
            this_thread::sleep_for(chrono::nanoseconds(time_sleep));
            continue;
        }
        break;
    }
    clock_t end = clock();
    int time = (end - start) / (CLOCKS_PER_SEC / 1000);
   	printf("Time: %d ms\n", time);

}

void create_thread(int time_sleep, int NumThreads, int counter_type) {
    if (counter_type == 1) {
        cout << "Mutex counter, " << NumThreads << " threads, " << time_sleep << "ns sleep\n";
    }
    else if (counter_type == 2) {
        cout << "Atomic counter, " << NumThreads << " threads, " << time_sleep << "ns sleep\n";
    }
    for (int i = 0; i < TaskNum; i++) {
        bytes[i] = 0;
    }
    thread* threads = new std::thread[NumThreads];
    if (counter_type == 1) {
        Mutex.lock();
        counter_mutex = 0;
        Mutex.unlock();
    }
    else if (counter_type == 2) {
        counter_atomic.store(0);
    }
    for (int i = 0; i < NumThreads; i++) {
        threads[i] = std::thread(thread_action, time_sleep, counter_type);
    }
    for (int i = 0; i < NumThreads; i++) {
        threads[i].join();
    }
    bool check = true;
    for (int i = 0; i < TaskNum; i++) { 
        if (bytes[i] != 1) {
            check = false;
        }
    }
    if (check) {
        cout << "Array is correct\n\n";
    }
    else cout << "Array is NOT correct\n\n";
    
}

int main()
{
    int NumThreads[4] = { 4, 8, 16, 32 };
    for (int i : NumThreads) {
        create_thread(0, i, 1); // 1 - mutex, 2 - atomic
        create_thread(0, i, 2);
        create_thread(10, i, 1);
        create_thread(10, i, 2);
    }
}
