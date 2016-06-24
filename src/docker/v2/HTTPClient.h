
#pragma once

#include "Logger.h"

#include <boost/asio.hpp>
#include <string>

#include <memory>
#include <type_traits>

namespace Sphinx {
namespace Docker {
namespace v2 {

typedef boost::asio::ip::tcp::socket TCPSocket;
typedef boost::asio::local::stream_protocol::socket UnixSocket;
template <typename T> struct Endpoint {
};

template <> struct Endpoint<UnixSocket> {
  using type = boost::asio::local::stream_protocol::endpoint;
};

template <> struct Endpoint<TCPSocket> {
  using type = boost::asio::ip::tcp::endpoint;
};

template <typename T>
class HTTPClient : public std::enable_shared_from_this<HTTPClient<T>> {
private:
  using Socket = T;
  using std::enable_shared_from_this<HTTPClient<T>>::shared_from_this;
  boost::asio::io_service &io_service_;
  Socket socket_;

  typename Endpoint<T>::type endpoint_;

  boost::asio::streambuf request_;
  boost::asio::streambuf response_;

public:
  template <typename U = Socket,
            typename = std::enable_if_t<std::is_same<U, TCPSocket>::value>>
  HTTPClient(boost::asio::io_service &io_service,
             const std::string &address,
             unsigned short port)
    : io_service_(io_service),
      socket_(io_service_),
      endpoint_(boost::asio::ip::address::from_string(address), port)
  {
    logger->trace("HTTPClient<TCPSocket>::HTTPClient: enter and exit");
  }

  template <typename U = Socket,
            typename = std::enable_if_t<std::is_same<U, UnixSocket>::value>>
  HTTPClient(boost::asio::io_service &io_service,
             const std::string &socket_path)
    : io_service_(io_service),
      socket_(io_service_),
      endpoint_(socket_path)
  {
    logger->trace("HTTPClient<UnixSocket>::HTTPClient: enter and exit");
  }

  auto get(const std::string &path)
  {
    logger->trace("HTTPClient::get: enter");

    auto self = shared_from_this();
    socket_.async_connect(
        endpoint_,
        [this, path, self](const boost::system::error_code &error_code) {
          self->handle_connect(error_code, path);

        });

    io_service_.run();
    logger->trace("HTTPClient::get: exit");
    return "";
  }

private:
  void handle_connect(const boost::system::error_code &error_code,
                      const std::string & /*path*/)
  {
    logger->trace("HTTPClient::handle_connect: enter");
    if (error_code) {
      logger->error("{0}: {1}", error_code.value(), error_code.message());
      logger->trace("HTTPClient::handle_connect: exit with error");
      return;
    }
    logger->trace("HTTPClient::handle_connect: exit");
  }

private:
  Logger logger = Sphinx::make_logger("HTTPClient");
};

} // namespace v2
} // namespace Sphinx
} // namespace Docker
