
#pragma once

#include "Logger.h"

#include "HTTPRequest.h"
#include "HTTPResponse.h"

#include <boost/asio.hpp>
#include <boost/endian/conversion.hpp>
#include <string>

#include <memory>
#include <type_traits>

#include "utils.h"

#include <typeinfo>

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
  boost::asio::io_service io_service_;
  std::shared_ptr<Socket> socket_;

  typename Endpoint<T>::type endpoint_;

  boost::asio::streambuf request_;
  boost::asio::streambuf response_;

  HTTPRequest http_request_;
  HTTPResponse http_response_;

  std::size_t content_data_left_;
  std::size_t content_length_;
  std::size_t chunk_data_left_;
  std::size_t chunk_size_;

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

  auto post(const std::string &path,
            const std::string &data = "",
            const HTTPHeaders &headers = {})
  {
    io_service_.reset();
    socket_ = std::make_shared<Socket>(io_service_);

    http_request_ = HTTPRequest{HTTPMethod::POST, path, data, headers};
    http_response_ = HTTPResponse{};
    response_.consume(response_.size());
    assert(response_.size() == 0);

    std::ostream request_stream(&request_);
    request_stream << http_request_.to_string();

    auto self = shared_from_this();
    socket_->async_connect(endpoint_, [this, path, self](auto &error_code) {
      handle_connect(error_code);

    });

    io_service_.run();
    return http_response_;
  }

  auto get(const std::string &path)
  {
    io_service_.reset();
    socket_ = std::make_shared<Socket>(io_service_);

    http_request_ = HTTPRequest{HTTPMethod::GET, path};
    http_response_ = HTTPResponse{};
    response_.consume(response_.size());
    assert(response_.size() == 0);

    std::ostream request_stream(&request_);
    request_stream << http_request_.to_string();

    auto self = shared_from_this();
    socket_->async_connect(endpoint_, [this, path, self](auto &error_code) {
      handle_connect(error_code);

    });

    io_service_.run();
    return http_response_;
  }

private:
  void handle_connect(const boost::system::error_code &error_code)
  {
    if (error_code) {
      logger->error("{0}: {1}", error_code.value(), error_code.message());
      return;
    }

    auto self = shared_from_this();
    boost::asio::async_write(*socket_, request_,
                             [this, self](auto &error_code, auto length) {
                               handle_write_request(error_code, length);

                             });
  }

  void handle_write_request(const boost::system::error_code &error_code,
                            std::size_t /*length*/)
  {
    if (error_code) {
      logger->error("{0}: {1}", error_code.value(), error_code.message());
      return;
    }

    auto self = shared_from_this();
    boost::asio::async_read_until(*socket_, response_, "\r\n",
                                  [this, self](auto &error_code, auto length) {
                                    handle_read_status_line(error_code, length);

                                  });
  }

  void handle_read_status_line(const boost::system::error_code &error_code,
                               std::size_t length)
  {
    if (error_code) {
      logger->error("{0}: {1}", error_code.value(), error_code.message());
      return;
    }

    std::string line = get_n_from_response_stream(length);
    http_response_.parse_status_line(line);

    auto self = shared_from_this();
    boost::asio::async_read_until(*socket_, response_, "\r\n\r\n",
                                  [this, self](auto &error_code, auto length) {
                                    handle_read_headers(error_code, length);

                                  });
  }

  void handle_read_headers(const boost::system::error_code &error_code,
                           std::size_t length)
  {
    if (error_code) {
      logger->error("{0}: {1}", error_code.value(), error_code.message());
      return;
    }
    std::string headers = get_n_from_response_stream(length);
    http_response_.parse_headers(headers);

    auto content_type = get_content_type(http_response_.headers());
    if (content_type == "application/json") {
      receive_application_json();
    }
    else if (content_type == "application/vnd.docker.raw-stream") {
      receive_application_docker_raw_stream();
    }
  }

  void receive_application_docker_raw_stream()
  {
    logger->trace("receive_application_docker_raw_stream");
    async_read_docker_raw_stream_header();
  }

  void async_read_docker_raw_stream_header()
  {
    logger->trace("async_read_docker_raw_stream_header");
    const std::size_t header_length = 8;
    auto self = shared_from_this();
    boost::asio::async_read(
        *socket_, response_, boost::asio::transfer_exactly(header_length),
        [this, self](auto &error_code, auto length) {
          handle_read_docker_raw_stream_header(error_code, length);
        });
  }

  void handle_read_docker_raw_stream_header(
      const boost::system::error_code &error_code, std::size_t /*length*/)
  {
    logger->trace("handle_read_docker_raw_stream_header");
    bool is_eof = error_code == boost::asio::error::eof;

    const std::size_t header_length = 8;
    if (error_code && !is_eof) {
      logger->error("{0}: {1}", error_code.value(), error_code.message());
      return;
    }
    if (is_eof && response_.size() == 0) {
      return;
    }
    auto header = get_n_from_response_stream(header_length);
    auto stream_type = static_cast<StreamType>(header[0]);
    auto data_size = boost::endian::big_to_native(
        *reinterpret_cast<std::uint32_t *>(&header[4]));

    logger->trace("Size of the data: {}", data_size);

    async_read_docker_raw_stream_data(stream_type, data_size);
  }

  void async_read_docker_raw_stream_data(const StreamType &stream_type,
                                         std::size_t data_size)
  {
    logger->trace("async_read_docker_raw_stream_data({}, {})",
                  static_cast<uint32_t>(stream_type), data_size);
    auto self = shared_from_this();
    boost::asio::async_read(
        *socket_, response_, boost::asio::transfer_exactly(data_size),
        [this, self, stream_type, data_size](auto &error_code, auto length) {
          handle_read_docker_raw_stream_data(error_code, length, stream_type,
                                             data_size);
        });
  }

  void handle_read_docker_raw_stream_data(
      const boost::system::error_code &error_code,
      std::size_t /*length*/,
      StreamType /*stream_type*/,
      std::size_t data_size)
  {

    logger->trace("handle_read_docker_raw_stream_data");
    bool is_eof = error_code == boost::asio::error::eof;

    if (error_code && !is_eof) {
      logger->error("{0}: {1}", error_code.value(), error_code.message());
      return;
    }

    auto data = get_n_from_response_stream(data_size);
    http_response_.append_data(data);

    if (!is_eof || response_.size() != 0) {
      async_read_docker_raw_stream_header();
    }
  }

  void receive_application_json()
  {
    if (is_chunked(http_response_.headers())) {
      content_length_ = 0;
      async_read_chunk_begin();
    }
    else {
      content_length_ = get_content_length(http_response_.headers());
      content_data_left_ = content_length_ - response_.size();
      async_read_content(content_data_left_);
    }
  }

  void async_read_chunk_begin()
  {
    auto self = shared_from_this();
    boost::asio::async_read_until(*socket_, response_, "\r\n",
                                  [this, self](auto &error_code, auto length) {
                                    handle_read_chunk_begin(error_code, length);
                                  });
  }

  void handle_read_chunk_begin(const boost::system::error_code &error_code,
                               std::size_t length)
  {
    if (error_code) {
      logger->error("{0}: {1}", error_code.value(), error_code.message());
      return;
    }
    auto data = get_n_from_response_stream(length);
    chunk_size_ = static_cast<std::size_t>(std::stoi(data, 0, 16));

    if (chunk_size_ + 2 >
        response_.size()) { // data in the chunk ends with CRLF
      chunk_data_left_ = chunk_size_ + 2 - response_.size();
      async_read_chunk_data(chunk_data_left_);
    }
    else {
      http_response_.headers()
          .remove("Transfer-Encoding")
          .add_header("Content-Length", content_length_);
    }
  }

  void async_read_chunk_data(std::size_t length)
  {
    auto self = shared_from_this();
    boost::asio::async_read(*socket_, response_,
                            boost::asio::transfer_at_least(length),
                            [this, self](auto &error_code, auto length) {
                              handle_read_chunk_data(error_code, length);
                            });
  }

  void handle_read_chunk_data(const boost::system::error_code &error_code,
                              std::size_t /*length*/)
  {
    bool is_eof = error_code == boost::asio::error::eof;

    if (error_code && !is_eof) {
      logger->error("{0}: {1}", error_code.value(), error_code.message());
      return;
    }
    if (chunk_size_ + 2 >
        response_.size()) { // data in the chunk ends with CRLF
      chunk_data_left_ = chunk_size_ + 2 - response_.size();
      async_read_chunk_data(chunk_data_left_);
    }
    else {
      content_length_ += chunk_size_;
      auto data = get_n_from_response_stream(chunk_size_);
      http_response_.append_data(data);
      get_n_from_response_stream(2); // consume CRLF

      async_read_chunk_begin();
    }
  }

  auto get_n_from_response_stream(std::size_t n)
  {
    assert(response_.size() >= n);
    std::istream response_stream(&response_);
    std::string data;
    data.reserve(n);
    std::copy_n(std::istreambuf_iterator<char>(response_stream), n,
                std::back_inserter(data));
    response_stream.ignore(1); // discard the last element since copy_n wont
                               // increase the iterator after reading the last
                               // one
    return data;
  }

  void async_read_content(std::size_t bytes_to_read)
  {
    auto self = shared_from_this();
    boost::asio::async_read(*socket_, response_,
                            boost::asio::transfer_at_least(bytes_to_read),
                            [this, self](auto &error_code, auto length) {
                              handle_read_content(error_code, length);
                            });
  }

  void handle_read_content(const boost::system::error_code &error_code,
                           std::size_t /*length*/)
  {
    bool is_eof = error_code == boost::asio::error::eof;

    if (error_code && !is_eof) {
      logger->error("{0}: {1}", error_code.value(), error_code.message());
      return;
    }

    if (response_.size() >= content_length_) {
      http_response_.append_data(get_n_from_response_stream(content_length_));
      content_data_left_ = 0;
    }
    else {
      content_data_left_ = content_length_ - response_.size();
      async_read_content(content_data_left_);
    }
  }

  auto is_chunked(const HTTPHeaders &headers)
  {
    auto transfer_encoding_header = headers.get("Transfer-Encoding");

    if (transfer_encoding_header &&
        transfer_encoding_header.value() == "chunked") {
      return true;
    }
    return false;
  }

  auto get_content_length(const HTTPHeaders &headers)
  {
    auto content_length_header = headers.get("Content-Length");
    std::size_t content_length = 0;

    if (content_length_header) {
      content_length =
          static_cast<std::size_t>(std::stoi(*content_length_header));
    }
    return content_length;
  }

  auto get_content_type(const HTTPHeaders &headers)
  {
    auto content_type_header = headers.get("Content-Type");
    std::string content_type;

    if (content_type_header) {
      content_type = *content_type_header;
    }
    return content_type;
  }

private:
  Logger logger = Sphinx::make_logger("HTTPClient");
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
