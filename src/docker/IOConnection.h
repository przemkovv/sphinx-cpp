
#include <string>
#include <boost/asio.hpp>

#include <type_traits>

namespace Sphinx {
namespace Docker {

typedef boost::asio::ip::tcp::socket TCPSocket;
typedef boost::asio::local::stream_protocol::socket UnixSocket;

template <typename T>
class IOConnection {
    private:
        using Socket = T;
        boost::asio::io_service svc;
        Socket socket;

        IOConnection() : socket(svc) {}


    public:
        template <typename U = Socket, typename = std::enable_if_t<std::is_same<U, TCPSocket>::value>>
        IOConnection(const std::string& address, unsigned short port)
            : IOConnection()
        {
            socket.connect({boost::asio::ip::address::from_string(address), port});
        }

        template <typename U = Socket, typename = std::enable_if_t<std::is_same<U, UnixSocket>::value>>
        IOConnection(const std::string& socket_path)
            : IOConnection()
        {
            socket.connect({socket_path});
        }

        void send(const std::string& data)
        {
            socket.send(boost::asio::buffer(data));
        }

        std::string receive()
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
    private:
        /* data */
};
} // namespace Sphinx 
} // namespace Docker 
