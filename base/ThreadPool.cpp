#include "ThreadPool.h"
#include <memory>
#include <mutex>
#include <iostream>
#include <thread>



namespace BASE 
{

/**
* @brief 开启线程池
* @param threadNum 线程池开启多少个线程
* @return 是否成功开启
*/
bool ThreadPool::start(int threadNum)
{
    if (started_) 
    {
        return false;
    }

    std::unique_lock<std::mutex> lock(mutex_);
    stop_ = false;
    started_ = true;

    workers_.clear();
    workers_.resize(threadNum);

    for (int i = 0; i < threadNum; ++i) 
    {
        workers_[i] = std::thread([this] {
           run(); 
        });
    }

    return true;
}


/**
* @brief  停止线程
*/
void ThreadPool::stopThreads()
{
    if (stop_) 
        return;

    stop_ = true;
    started_ = false;

    cond_.notify_all();

    for (auto& worker  : workers_) {
        worker.join();
    }

    tasks_.clear();
}


/**
* @brief   向线程池投递一个任务
* @param task 任务
*/
void ThreadPool::post(Task task)
{
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if (stop_) 
            return; 

        tasks_.push_back(std::move(task));
    }
    cond_.notify_one();
}


/**
* @brief  直接执行该任务
* @param task 任务
*/
void ThreadPool::dispatch(Task task)
{
    try 
    {
        task();
    } 
    catch (const std::exception& e) 
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    } 
    catch (...)
    {
        std::cerr << "Unknown exception in thread pool task\n";
    }
}



/**
* @brief   从任务队列中获取一个任务
* @param task 任务
* @return 是否成功获取
*/
bool ThreadPool::getTask(Task& task)
{

    std::unique_lock<std::mutex> lock(mutex_);


    while (!stop_ && tasks_.empty())
    {
        cond_.wait(lock);
    }

    if (stop_ && tasks_.empty()) 
    {
        return false;
    }


    task = tasks_.front();
    tasks_.pop_front();

    return true;
}



/**
* @brief 线程的运行函数
*/
void ThreadPool::run()
{
    while (!stop_ || taskSize()) 
    {
        Task task;

        if (getTask(task))
        {   
            dispatch(task);
        }
    }
}




}