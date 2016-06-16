
#include "TCPConnection.h"

namespace Sphinx {
namespace Docker {

TCPConnection::TCPConnection(const std::string& address, unsigned short port)
    : socket{svc}
{
    socket.connect({boost::asio::ip::address::from_string(address), port});
}

void TCPConnection::send(const std::string& data)
{
    socket.send(boost::asio::buffer(data));
}

std::string TCPConnection::receive()
{
    boost::system::error_code ec;
    std::string data;

    do {
        char buf[1024];
        size_t bytes_transferred = socket.receive(boost::asio::buffer(buf), {}, ec);

        if (!ec) { data.append(buf, buf + bytes_transferred); }
    } while (!ec);

    return data;
}
} // namespace Docker
} // namespace Sphinx
