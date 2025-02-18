#pragma once

#include <algorithm>
#include <cstddef>
#include <functional>
#include <iterator>
#include <vector>
#include <memory>
#include <string>

namespace BASE 
{

/**
 * @class Buffer
 * @brief 缓冲区类
 */
class Buffer
{
public:
    typedef std::shared_ptr<Buffer> ptr;
    typedef std::function<void(char *, const std::size_t)> OpFunc;
    typedef std::function<bool (ptr)> PreFun;
public:

    /**
    * @brief 构造函数
    * @param size 期望分配的内存大小
    */
    Buffer(const std::size_t size = 0) : pos_(0)
    {
        alloc(size);
    }

    /**
    * @brief 构造函数
    * @param str 数据被初始化成str
    */
    Buffer(const std::string& str) : pos_(0)
    {
        *this = str;
    }

    /**
    * @brief 获取缓冲区读指针位置
    * @return 读指针位置
    */
    std::size_t transferred() const 
    {
        return pos_;
    }


    /**
    * @brief 获取缓冲区中可读数据的大小
    * @return 可读数据的大小
    */
    std::size_t validSize() const
    {
        return data_.size() - pos_;
    }

    /**
    * @brief 缓冲区最大可分配内存大小
    * @return 最大可分配内存大小
    */
    std::size_t getMaxSize() const
    {
        return data_.max_size();
    }


    /**
    * @brief  当前缓冲区的内存大小
    * @return 缓冲区的内存大小
    */
    std::size_t getCurSize() const
    {
        return data_.size();
    }

    /**
    * @brief 获取原始缓冲区
    * @return 原始缓冲区
    */
    std::vector<char>& rawData()
    {
        return data_;
    }

    /**
    * @brief 获取缓冲区的起始指针
    * @return 缓冲区的起始指针
    */
    const char * data() const
    {
        return data_.data();
    }

    /**
    * @brief 获取缓冲区中可读数据的起始位置
    * @return 可读数据的起始位置
    */
    char * memory()
    {
        return validSize() == 0 ? nullptr : &data_[pos_];
    }


    /**
    * @brief 提交数据
    */
    void commit(std::size_t len)
    {
        pos_ = len > validSize() ? data_.size() : len + pos_;

        if (pos_ < 0) 
        {
            pos_ = 0;
        }
    }

    /**
    * @brief 清空缓冲区
    */
    void clear()
    {
        pos_ = 0;
        std::fill(data_.begin(), data_.end(), '\0');
    }

    /**
    * @brief 获取数据
    */
    void get(const std::size_t& count, const OpFunc& func);
    
    /**
    * @brief 获取数据
    */
    void get(const std::size_t off, const std::size_t& count, const OpFunc& func);


    /**
    * @brief 缓冲区内存分配函数
    * @param size 期望分配的内存大小
    * @return 是否分配成功
    */
    bool alloc(std::size_t size);


    void operator+=(const std::string& str)
    {
        std::copy(str.begin(), str.end(), std::back_inserter(data_));
    }

    void operator=(const std::string& str)
    {
        data_.assign(str.begin(), str.end());
    }
private:
    std::vector<char> data_;        //数据
    std::size_t pos_;                       //下一个读取数据的位置
}; // Buffer
}