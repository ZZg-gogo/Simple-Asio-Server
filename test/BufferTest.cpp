#include <iostream>

#include "../base/Buffer.h"



int main(int argc, char ** argv)
{
    BASE::Buffer buf1(1024);
    std::cout<<"buf1.getCurSize() = "<<buf1.getCurSize()<<std::endl;

    std::cout<<"buf1.getMaxSize() = "<<buf1.getMaxSize()<<std::endl;

    std::cout<<"buf1.validSize() = "<<buf1.validSize()<<std::endl;

    BASE::Buffer buf2;

    std::cout<<"buf2.getCurSize() = "<<buf2.getCurSize()<<std::endl;

    std::cout<<"buf2.getMaxSize() = "<<buf2.getMaxSize()<<std::endl;

    std::cout<<"buf2.validSize() = "<<buf2.validSize()<<std::endl;


    BASE::Buffer buf3("Hello World");

    std::cout<<"buf3.getCurSize() = "<<buf3.getCurSize()<<std::endl;

    std::cout<<"buf3.getMaxSize() = "<<buf3.getMaxSize()<<std::endl;

    std::cout<<"buf3.validSize() = "<<buf3.validSize()<<std::endl;

    auto fun = [](char * str, int len){
        std::string s{str, str+len};
        std::cout<<"fun s= "<<s<<std::endl;
    };

    buf3.get(300, fun);

    buf3.get(5, 300, fun);

    std::cout<<"buf3 before commit = "<<buf3.memory()<<std::endl;
    buf3.commit(5);
    std::cout<<"buf3 after commit = "<<buf3.memory()<<std::endl;
    std::cout<<"buf3.validSize() = "<<buf3.validSize() <<" buf3.transferred= "<<buf3.transferred()<<std::endl;


    buf3 += " Hello";
    std::cout<<"buf3  "<<buf3.memory()<<std::endl;
    std::cout<<"buf3.validSize() = "<<buf3.validSize() <<" buf3.transferred= "<<buf3.transferred()<<std::endl;
    return 0;
}
