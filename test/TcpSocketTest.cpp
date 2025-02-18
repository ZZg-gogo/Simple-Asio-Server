#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address.hpp>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <boost/any.hpp>
#include <memory>
#include "../net/TcpSocket.h"
#include "../base/Buffer.h"



void WriteHandler(const boost::any& any, const boost::system::error_code&,
    const std::size_t& bytesTransferred, const BASE::Buffer::ptr&)
{
    NET::TcpSocket::ptr Tcpsocket = boost::any_cast<NET::TcpSocket::ptr>(any);
    Tcpsocket->getParam([](const BASE::Param::ptr param){
        std::cout <<"WriteHandler uid="<<param->uniqueId_<<std::endl;
    });
}


void ReadHandler(const boost::any& any, const boost::system::error_code&,
    const std::size_t& bytesTransferred, const BASE::Buffer::ptr& buf)
{
    NET::TcpSocket::ptr Tcpsocket = boost::any_cast<NET::TcpSocket::ptr>(any);
    Tcpsocket->getParam([](const BASE::Param::ptr param){
        std::cout <<"ReadHandler uid="<<param->uniqueId_<<std::endl;
    });


    std::cout <<"bytesTransferred="<<bytesTransferred<<"  buf="<<buf->data()<<std::endl;
}


int main(int argc, char ** argv)
{

    if (argc != 3) 
    {
        std::cerr <<"Usage: TcpSocket need Host and Port"<<std::endl;
        return 0;
    }


    try 
    {
    
        boost::asio::io_context ioc;
        auto TcpSocket =  std::make_shared<NET::TcpSocket>(ioc);
        auto socket = TcpSocket->getSocket();
        
        boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(argv[1]), 
        std::atoi(argv[2]));

        socket->async_connect(ep, [TcpSocket](const boost::system::error_code& error){
            if (error) 
            {
                std::cerr <<"async_connect error"<<std::endl;
                return ;
            }
            TcpSocket->accept();
            TcpSocket->asyncWrite(WriteHandler, "Hello World");
            
            BASE::Buffer::ptr buffer = std::make_shared<BASE::Buffer>(sizeof("Hello World"));

            //{ echo "Welcome to the server!"; cat; } | nc -l -p 8888 测试命令
            TcpSocket->asyncRead(ReadHandler, buffer, buffer->getCurSize(), 0, 
            [](BASE::Buffer::ptr buffer){
                return false;
            });
            
        });


        ioc.run();
    } catch (const std::exception& e) 
    {
        std::cerr<<"err = "<<e.what()<<std::endl;
    }






    
    return 0;
}