#pragma  once

#include <boost/asio.hpp>
#include <boost/any.hpp>
#include "Buffer.h"


namespace BASE 
{
    typedef std::function<void (const boost::any& any, const boost::system::error_code&,
        const std::size_t& bytesTransferred, const Buffer::ptr&)> ClientHandler;
}