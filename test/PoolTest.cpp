#include <atomic>
#include <iostream>
#include <memory>
#include <thread>
#include "../base/Pool.h"
#include "../base/ThreadPool.h"

class Test 
{
public:
    Test(const std::string& s) : val(++Count), name(s+ std::to_string(val)) 
    {
        std::cout << " this= "<<this<<"Constructed: " << name << " with value " << val << std::endl;
    }
    ~Test() 
    {
        std::cout << " this= "<<this<< "Destroyed: " << name << " with value " << val << std::endl;
    }
    void print() 
    { 
        std::cout << " this= "<<this<<" Thread id = " <<std::this_thread::get_id()<<" Value: "
             << val << ", Name: " << name << std::endl; 
        //std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    static std::atomic_int Count;
private:
    int val;
    std::string name;
};


std::atomic_int Test::Count = 0;



void func(const BASE::Pool<Test>::ptr& pool)
{
    BASE::Pool<Test>::ObjPtr obj;
    while (true) 
    {
        obj = pool->get();
        obj->print();

        pool->push(obj);

        std::this_thread::yield();
    }    
}


int main(int argc, char* argv[])
{
    int index = 0;
    std::string s {"zzg"};
    

    BASE::ThreadPool threadPool;
    threadPool.start(4);

    BASE::Pool<Test>::ptr objPool = std::make_shared<BASE::Pool<Test>>([&s]()->BASE::Pool<Test>::ObjPtr{
        return  std::make_shared<Test>(s);
    } ,1, 3);

    for (int i = 0; i < 4; ++i) 
    {
        threadPool.post([objPool]{
            func(objPool);
        });
    }

    
    
    return 0;
}