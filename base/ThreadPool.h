#pragma once

#include <algorithm>
#include <functional>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <thread>
#include <atomic>
#include <boost/noncopyable.hpp>

namespace  BASE 
{


/**
 * @class ThreadPool
 * @brief 线程池类
 */
class ThreadPool : boost::noncopyable
{
public:
    typedef std::shared_ptr<ThreadPool> ptr;
    typedef std::function<void()> Task;
public:
    /**
     * @brief 默认构造函数
     */
    ThreadPool()
    {
        stop_ = true;
        started_ = false;
    }

    /**
     * @brief 析构函数
     */
    ~ThreadPool()
    {
        stopThreads();
    }

    /**
     * @brief 开启线程池
     * @param threadNum 线程池开启多少个线程
     * @return 是否成功开启
     */
    bool start(int threadNum = std::max(std::thread::hardware_concurrency(), 1u)*2);

    /**
     * @brief   向线程池投递一个任务
     * @param task 任务
     */
    void post(Task task);

    /**
     * @brief  直接执行该任务
     * @param task 任务
     */
    void dispatch(Task task);

    /**
     * @brief 线程池是否已经停止
     */
    bool stopped() const { return stop_; }

    /**
     * @brief 线程池是否已经启动
     */
    bool started() const { return started_; }

    /**
     * @brief 线程池是否已经启动
     * @return 当前任务的数量
     */
    std::size_t taskSize()  
    { 
        std::unique_lock<std::mutex> lock(mutex_);
        return tasks_.size(); 
    }

    /**
     * @brief 线程数量
     * @return 当前线程数量
    */
    std::size_t threadSize() 
    {
        std::unique_lock<std::mutex> lock(mutex_);
        return workers_.size(); 
    }
private:
    /**
     * @brief 线程的运行函数
     */
    void run();

    /**
     * @brief  停止线程
     */
    void stopThreads();

    /**
     * @brief   从任务队列中获取一个任务
     * @param task 任务
     * @return 是否成功获取
     */
    bool getTask(Task& task);
private:
    std::atomic_bool  stop_;
    std::atomic_bool  started_;
    std::mutex mutex_;
    std::condition_variable cond_;
    std::deque<Task> tasks_;
    std::vector<std::thread> workers_;
    
};

}