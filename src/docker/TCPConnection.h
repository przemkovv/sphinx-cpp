
#pragma once

#include <boost/asio.hpp>
#include <string>

namespace Sphinx {
namespace Docker {

class TCPConnection {
        boost::asio::io_service svc;
        boost::asio::ip::tcp::socket socket;

    public:
        TCPConnection(const std::string& address, unsigned short port);
        void send(const std::string& data);
        std::string receive();

};

} // namespace Docker
} // namespace Sphinx
