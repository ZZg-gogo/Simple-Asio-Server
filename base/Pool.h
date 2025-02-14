#pragma once



#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <iostream>
#include <memory>
#include <vector>
#include <functional>
#include <mutex>
#include <deque>

namespace BASE
{

template<typename T>
class Pool
{
public:
    typedef std::shared_ptr<Pool<T>> ptr;
    typedef std::shared_ptr<T> ObjPtr;
public:
    Pool(std::function<ObjPtr()> constructor,std::size_t initSize, std::size_t maxSize):
        initSize_(initSize),
        maxSize_(maxSize),
        usedCount_(0),
        new_(constructor)
    {
        std::unique_lock<std::mutex> lock(mutex_);

        for(int i = 0; i < initSize_; ++i)
        {
            freeObjs_.push_back(new_());
        }
    }


    ObjPtr get()
    {
        std::unique_lock<std::mutex> lock(mutex_);

        while (freeObjs_.empty() && usedCount_ >= maxSize_) 
        {
            cond_.wait(lock);
        }

        if (!freeObjs_.empty()) 
        {
            ObjPtr obj = freeObjs_.front();
            freeObjs_.pop_front();
            ++usedCount_;
            return obj;
        }

        ++usedCount_;
        return new_();
    }



    void push(ObjPtr obj)
    {
        std::unique_lock<std::mutex> lock(mutex_);

        freeObjs_.push_back(obj);
        --usedCount_;    
        cond_.notify_one();
    }
private:

private:
    std::mutex mutex_;
    std::condition_variable cond_;
    std::size_t initSize_;
    std::size_t maxSize_;
    std::size_t usedCount_;                             //已经有多少个对象被使用了
    std::deque<ObjPtr> freeObjs_;                      //空闲对象
    std::function<ObjPtr()> new_;
};




}