#pragma once

#include <string>
#include <memory>

namespace BASE 
{
class Param
{
public: 
    typedef std::shared_ptr<Param> ptr;
    
public:
    Param()
    {
        reset();
    }

    void reset()
    {
        uniqueId_.clear();
        data_.clear();
        binary_.clear();
        header_.clear();
        cache_.clear();

        reserve_ = 0;
        length_ = 0;
        type = 0;
        reserve_ = false;
    }
private:

    std::string uniqueId_;
    std::string data_;
    std::string binary_;
    std::string header_;
    std::string cache_;
    int reserve_;
    int length_;    
    int type;
    bool headReceived_;
};
}