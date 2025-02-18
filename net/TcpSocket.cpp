#include "TcpSocket.h"
#include "../base/util.h"
#include <boost/asio/buffer.hpp>
#include <boost/asio/execution_context.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/strand.hpp>
#include <chrono>
#include <functional>
#include <memory>
#include <mutex>



namespace NET
{

/**
* @brief 构造函数
* @param ioc boost::asio::io_context对象
*/
TcpSocket::TcpSocket(boost::asio::io_context& ioc) :
    socket_(std::make_shared<boost::asio::ip::tcp::socket>(ioc)),
    timer_(std::make_shared<boost::asio::steady_timer>(ioc)),
    strand_(std::make_shared<boost::asio::strand<boost::asio::io_context::executor_type>>(boost::asio::make_strand(ioc.get_executor()))),
    param_(std::make_shared<BASE::Param>())
{
}


void TcpSocket::accept()
{
    std::unique_lock<std::mutex> lock(paramMutex_);

    param_->reset();
    
    boost::system::error_code ec;
    param_->uniqueId_ = socket_->remote_endpoint(ec).address().to_string(ec) +
        ":" +
        BASE::ToString< unsigned short>(socket_->remote_endpoint(ec).port());
}

void TcpSocket::free()
{
    std::unique_lock<std::mutex> lock(mutex_);

    cancelTimer();
    close_();
    param_->reset();
}


void TcpSocket::close_()
{
    if (!socket_->is_open()) 
    {
        return;
    }
    boost::system::error_code ec;
    socket_->shutdown(boost::asio::socket_base::shutdown_both, ec);
    socket_->close(ec);
}

void TcpSocket::cancelTimer()
{
    boost::system::error_code ec;
    timer_->cancel(ec);
}


/**
* @brief 异步读取函数
* @param handler 读取回调函数
* @param buffer 读取函数区
* @param exceptSize 期望读取的字节数
* @param milliseconds 超时时间
*/
void TcpSocket::asyncRead(const BASE::ClientHandler& handler,
    BASE::Buffer::ptr buffer,
    const std::size_t exceptSize,
    const uint64_t milliseconds,
    BASE::Buffer::PreFun preBuffer
)
{
    std::unique_lock<std::mutex> lock(mutex_);

    std::size_t exceptRead = exceptSize;
    if (exceptRead > buffer->getCurSize()) {
        exceptRead = buffer->getCurSize();
    }
    

    socket_->async_read_some(boost::asio::buffer(buffer->memory(), exceptRead), 
     boost::asio::bind_executor(*strand_, [self=shared_from_this(),
        buffer,
        handler,
        exceptRead,
        preBuffer]
        (const boost::system::error_code& error, std::size_t bytes_transferred){
            self->handleRead(error, bytes_transferred, handler, buffer, exceptRead, preBuffer);
        })
    );


    if (milliseconds > 0) 
    {
        timer_->expires_after(std::chrono::milliseconds(milliseconds));
        timer_->async_wait(
            boost::asio::bind_executor(*strand_, [self=shared_from_this(),
            milliseconds]
            (const boost::system::error_code& error){
                self->handleTimeout(error, milliseconds);
            })
        );
    }
}


/**
* @brief 异步写入函数
* @param handler 写入回调函数
* @param buffer 写入缓冲区
*/
void TcpSocket::asyncWrite(const BASE::ClientHandler& handler,
    BASE::Buffer::ptr buffer
)
{
    std::unique_lock<std::mutex> lock(mutex_);

    socket_->async_write_some(boost::asio::buffer(buffer->memory(), buffer->getCurSize()),
        boost::asio::bind_executor(*strand_, [self = shared_from_this(),
            handler,
            buffer
        ](boost::system::error_code error, std::size_t bytes){
            self->handleWrite(error, bytes, handler, buffer);
        })

    );
}


void TcpSocket::getParam(const std::function<void(const BASE::Param::ptr)>& handler)
{
    std::unique_lock<std::mutex> lock(paramMutex_);
    handler(param_);
}

/**
* @brief 异步写入函数
* @param handler 写入回调函数
* @param str 写入字符串 
*/
void TcpSocket::asyncWrite(const BASE::ClientHandler& handler,
    const std::string& str
)
{
    BASE::Buffer::ptr buffer = std::make_shared<BASE::Buffer>(str);
    asyncWrite(handler, buffer);
}



void TcpSocket::handleRead(const boost::system::error_code&   error,
    const std::size_t bytes_transferred,
    const BASE::ClientHandler& handler,
    BASE::Buffer::ptr buffer,
    const std::size_t exceptSize,
    BASE::Buffer::PreFun preBuffer
)
{
    buffer->commit(bytes_transferred);

    if (error || !bytes_transferred || buffer->transferred() >= exceptSize || preBuffer(buffer)) 
    {
        cancelTimer();  //取消定时器
        handler(shared_from_this(), error, bytes_transferred, buffer);
    }
    else
    {
        socket_->async_read_some(boost::asio::buffer(buffer->memory(), exceptSize-bytes_transferred), 
        boost::asio::bind_executor(*strand_, [self=shared_from_this(),
           buffer,
           handler,
           exceptSize,
           preBuffer]
           (const boost::system::error_code& error, std::size_t bytes_transferred){
               self->handleRead(error, bytes_transferred, handler, buffer, exceptSize, preBuffer);
           })
       );
    }
}


void TcpSocket::handleWrite(const boost::system::error_code&   error,
    const std::size_t bytes_transferred,
    const BASE::ClientHandler& handler,
    BASE::Buffer::ptr buffer
)
{
    buffer->commit(bytes_transferred);
    if (error || !bytes_transferred || bytes_transferred >= buffer->getCurSize()) 
    {
        handler(shared_from_this(), error, (bytes_transferred ? buffer->transferred() : bytes_transferred), buffer);
    }
    else
    {
        socket_->async_write_some(boost::asio::buffer(buffer->memory(), buffer->validSize()),
        boost::asio::bind_executor(*strand_, [self = shared_from_this(),
            handler,
            buffer
            ](boost::system::error_code error, std::size_t bytes){
                self->handleWrite(error, bytes, handler, buffer);
            })
        );
    }
}

void TcpSocket::handleTimeout(const boost::system::error_code&   error,
    const uint64_t milliseconds
)
{
    if (error) 
    {
        return;
    }

    //超时了
    if (timer_->expires_at() <= std::chrono::steady_clock::now()) 
    {
        cancelTimer();
        close_();
    }
    else if(milliseconds > 0)
    {
        timer_->expires_after(std::chrono::milliseconds(milliseconds));
        timer_->async_wait(
            boost::asio::bind_executor(*strand_, [self=shared_from_this(),
            milliseconds]
            (const boost::system::error_code& error){
                self->handleTimeout(error, milliseconds);
            })
        );
    }
}



}// namespace NET