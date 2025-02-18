#include <iostream>
#include <memory>
#include <functional>

class Test : public std::enable_shared_from_this<Test>
{
public:
    void fun()
    {
        std::cout<<"fun"<<std::endl;
    }
};


void TestSharedPtr(const std::shared_ptr<Test>& a)
{
    std::cout<<"a.use_count()"<<a.use_count()<<std::endl;
}


int main() 
{
    std::cout << "Hello, World!" << std::endl;

    std::shared_ptr<Test> a = std::make_shared<Test>();

    auto fun = std::bind(&Test::fun, a);
    std::cout<<"aaaaaaa.use_count()"<<a.use_count()<<std::endl;

    TestSharedPtr(a);
    fun();
    return 0;
}