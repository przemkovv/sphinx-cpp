
#include <string>
#include <boost/asio.hpp>

#include <type_traits>
#include <memory>

namespace Sphinx {
namespace Docker {

typedef boost::asio::ip::tcp::socket TCPSocket;
typedef boost::asio::local::stream_protocol::socket UnixSocket;
template<typename T>
struct Endpoint {
};

template<>
struct Endpoint<UnixSocket> {
    using type = boost::asio::local::stream_protocol::endpoint;
};

template<>
struct Endpoint<TCPSocket> {
    using type = boost::asio::ip::tcp::endpoint;
};

template <typename T>
class IOConnection {
    private:
        using Socket = T;
        std::unique_ptr<boost::asio::io_service> svc;
        Socket socket;

        typename Endpoint<T>::type endpoint;

    public:
        template <typename U = Socket, typename = std::enable_if_t<std::is_same<U, TCPSocket>::value>>
        IOConnection(const std::string& address, unsigned short port)
            : svc(std::make_unique<boost::asio::io_service>()),
              socket(*svc),
              endpoint(boost::asio::ip::address::from_string(address), port)
        {
        }

        template <typename U = Socket, typename = std::enable_if_t<std::is_same<U, UnixSocket>::value>>
        IOConnection(const std::string& socket_path)
            : svc(std::make_unique<boost::asio::io_service>()),
              socket(*svc),
              endpoint(socket_path)
        {
        }
        void close()
        {
            socket.close();
        }

        void connect()
        {
            socket.connect(endpoint);
        }

        void reconnect()
        {
            close();
            connect();
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
