#include <iostream>
#include <chrono>
#include "../base/ThreadPool.h"

using namespace BASE;

void testTask(int id)
{
    std::cout << "Task " << id << " started on thread " << std::this_thread::get_id() << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "Task " << id << " finished" << std::endl;
}

int main()
{
    ThreadPool pool;
    pool.start(4); // 启动 4 个线程

    for (int i = 0; i < 1000000; ++i)
    {
        pool.post([i]() { testTask(i); });
    }

    //std::this_thread::sleep_for(std::chrono::seconds(3)); // 让任务执行一会儿
    
    //pool.stopThreads(); // 关闭线程池
    std::cout << "Thread pool stopped" << std::endl;

    return 0;
}
