#pragma once

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/strand.hpp>
#include <memory>
#include "../base/Param.h"
#include "../base/Handler.h"

namespace NET 
{


/**
* @class TcpSocket
* @brief Tcp连接类
*/
class TcpSocket : public std::enable_shared_from_this<TcpSocket>    
{
public:
    typedef std::shared_ptr<TcpSocket> ptr;
    typedef  std::shared_ptr<boost::asio::ip::tcp::socket> SocketPtr;
    typedef std::shared_ptr<boost::asio::steady_timer> TimerPtr;
    typedef std::shared_ptr<boost::asio::strand<boost::asio::io_context::executor_type>> StrandPtr;
public:
    /**
    * @brief 构造函数
    * @param ioc boost::asio::io_context对象
    */
    TcpSocket(boost::asio::io_context& ioc);

    /**
    * @brief 默认析构函数
    */
    ~TcpSocket() = default;

    /**
    * @brief 对参数进行操作
    * @param handler 参数操作函数
    */
    void getParam(const std::function<void(const BASE::Param::ptr)>& handler);

    /**
    * @brief 获取socket对象
    * @return socket对象
    */
    SocketPtr getSocket()
    {
        return socket_;
    }

    /**
    * @brief 连接建立进行初始化
    */
    void accept();

    /**
    * @brief 关闭socket和timer
    */
    void free();

public:
    /**
    * @brief 异步读取函数
    * @param handler 读取回调函数
    * @param buffer 读取函数区
    * @param exceptSize 期望读取的字节数
    * @param milliseconds 超时时间
    */
    void asyncRead(const BASE::ClientHandler& handler,
        BASE::Buffer::ptr buffer,
        const std::size_t exceptSize,
        const uint64_t milliseconds,
        BASE::Buffer::PreFun preBuffer
    );


    /**
    * @brief 异步写入函数
    * @param handler 写入回调函数
    * @param buffer 写入缓冲区
    */
    void asyncWrite(const BASE::ClientHandler& handler,
        BASE::Buffer::ptr buffer
    );

    /**
    * @brief 异步写入函数
    * @param handler 写入回调函数
    * @param str 写入字符串 
    */
    void asyncWrite(const BASE::ClientHandler& handler,
        const std::string& str
    );
    
    

private:
    void handleRead(const boost::system::error_code&   error,
        const std::size_t bytes_transferred,
        const BASE::ClientHandler& handler,
        BASE::Buffer::ptr buffer,
        const std::size_t exceptSize,
        BASE::Buffer::PreFun preBuffer
    );


    void handleWrite(const boost::system::error_code&   error,
        const std::size_t bytes_transferred,
        const BASE::ClientHandler& handler,
        BASE::Buffer::ptr buffer
    );

    /**
    * @brief 定时器超时处理函数
    * @param error 错误码
    * @param milliseconds 超时时间
    */
    void handleTimeout(const boost::system::error_code&   error,
        const uint64_t milliseconds
    );

    /**
    * @brief 关闭socket
    */
    void close_();

    /**
    * @brief 取消定时器
    */
    void cancelTimer();

private:
    SocketPtr socket_;
    TimerPtr timer_;
    StrandPtr strand_;
    BASE::Param::ptr param_;

    std::mutex mutex_;
    std::mutex paramMutex_;
};


}