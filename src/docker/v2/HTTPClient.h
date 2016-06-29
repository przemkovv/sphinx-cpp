
#pragma once

#include "Logger.h"

#include "HTTPRequest.h"
#include "HTTPResponse.h"

#include <boost/asio.hpp>
#include <string>

#include <memory>
#include <stdexcept>

#include "utils.h"

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

  using Socket = T;


public:
  template <typename U = Socket,
            typename = std::enable_if_t<std::is_same<U, TCPSocket>::value>>
  explicit HTTPClient(const std::string &address, unsigned short port)
    : endpoint_(boost::asio::ip::address::from_string(address), port)
  {
  }

  template <typename U = Socket,
            typename = std::enable_if_t<std::is_same<U, UnixSocket>::value>>
  explicit HTTPClient(const std::string &socket_path) : endpoint_(socket_path)
  {
  }

  HTTPResponse request(HTTPMethod method,
                       const std::string &path,
                       const std::string &data = "",
                       const HTTPHeaders &headers = {});
  HTTPResponse post(const std::string &path,
                    const std::string &data = "",
                    const HTTPHeaders &headers = {});
  HTTPResponse get(const std::string &path,
                   const std::string &data = "",
                   const HTTPHeaders &headers = {});
  // INFO: delete name is restricted
  /*HTTPResponse delete(const std::string &path,
                    const std::string &data = "",
                    const HTTPHeaders &headers = {});*/
  HTTPResponse put(const std::string &path,
                   const std::string &data = "",
                   const HTTPHeaders &headers = {});

private:
  void prepare_request(const HTTPRequest &request);
  void prepare_response();
  void async_connect();

  void handle_connect(const boost::system::error_code &error_code);
  void handle_write_request(const boost::system::error_code &error_code,
                            std::size_t /*length*/);
  void handle_read_status_line(const boost::system::error_code &error_code,
                               std::size_t length);
  void handle_read_headers(const boost::system::error_code &error_code,
                           std::size_t length);

  void receive_application_docker_raw_stream();
  void async_read_docker_raw_stream_header();
  void handle_read_docker_raw_stream_header(
      const boost::system::error_code &error_code, std::size_t /*length*/);
  void async_read_docker_raw_stream_data(const StreamType &stream_type,
                                         std::size_t data_size);
  void handle_read_docker_raw_stream_data(
      const boost::system::error_code &error_code,
      std::size_t /*length*/,
      StreamType /*stream_type*/,
      std::size_t data_size);

  void receive_text();
  void async_read_chunk_begin();
  void handle_read_chunk_begin(const boost::system::error_code &error_code,
                               std::size_t length);
  void async_read_chunk_data(std::size_t length);
  void handle_read_chunk_data(const boost::system::error_code &error_code,
                              std::size_t /*length*/);
  void async_read_content(std::size_t bytes_to_read);
  void handle_read_content(const boost::system::error_code &error_code,
                           std::size_t /*length*/);

  auto get_n_from_response_stream(std::size_t n);

private:
  void log_error(const boost::system::error_code &error_code);
  Logger logger = Sphinx::make_logger("HTTPClient");
  Logger logger_request = Sphinx::make_logger("HTTPClient::request");
  Logger logger_response = Sphinx::make_logger("HTTPClient::response");

private:
  using std::enable_shared_from_this<HTTPClient<T>>::shared_from_this;

  boost::asio::io_service io_service_;
  std::shared_ptr<Socket> socket_;

  typename Endpoint<T>::type endpoint_;

  boost::asio::streambuf request_buffer_;
  boost::asio::streambuf response_buffer_;

  HTTPRequest http_request_;
  HTTPResponse http_response_;

  std::size_t content_data_left_;
  std::size_t content_length_;
  std::size_t chunk_data_left_;
  std::size_t chunk_size_;

  const std::size_t CRLF = 2;
};

inline auto make_http_client(const std::string &address, unsigned short port)
{
  return std::make_shared<HTTPClient<TCPSocket>>(address, port);
}
inline auto make_http_client(const std::string &socket_path)
{
  return std::make_shared<HTTPClient<UnixSocket>>(socket_path);
}

} // namespace v2
} // namespace Sphinx
} // namespace Docker
