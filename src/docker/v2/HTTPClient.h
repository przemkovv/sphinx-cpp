
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
  std::shared_ptr<Socket> socket_;

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
      socket_(std::make_shared<Socket>(io_service_)),
      endpoint_(boost::asio::ip::address::from_string(address), port)
  {
  }

  template <typename U = Socket,
            typename = std::enable_if_t<std::is_same<U, UnixSocket>::value>>
  HTTPClient(boost::asio::io_service &io_service,
             const std::string &socket_path)
    : io_service_(io_service),
      socket_(std::make_shared<Socket>(io_service_)),
      endpoint_(socket_path)
  {
  }

  auto get(const std::string &path) {

    auto self = shared_from_this();
    boost::asio::async_connect(
        socket_, endpoint_,
        [this, path, self](boost::system::error_code error_code, std::size_t) {
          handle_connect(error_code, path);

        });

    return "";
  }

private:
  void handle_connect(const boost::system::error_code /*error_code*/,
                      const std::string &path)
  {
    logger->info("{}", path);
  }

private:
  Logger logger = Sphinx::make_logger("HTTPClient");
};

} // namespace v2
} // namespace Sphinx
} // namespace Docker
