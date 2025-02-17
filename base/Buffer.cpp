#include "Buffer.h"


namespace BASE
{


/**
* @brief 缓冲区内存分配函数
* @param size 期望分配的内存大小
* @return 是否分配成功
*/

bool Buffer::alloc(std::size_t size)
{
    if (size <= 0) 
    {
        return true;
    }

    if (size > getMaxSize()) 
    {
        return false;
    }

    data_.resize(size);

    return true;
}

/**
* @brief 获取数据
*/
void Buffer::get(const std::size_t& count, const OpFunc& func)
{
    get(0, count, func);
}

/**
* @brief 获取数据
*/
void Buffer::get(const std::size_t off, const std::size_t& count, const OpFunc& func)
{
    if (off >= getCurSize() || count <= 0) 
    {
        return;
    }

    
    std::size_t minLen = (off+count) > getCurSize() ? getCurSize() - off : count;

    func(data_.data()+ off, minLen);
}




}// namespace BASE