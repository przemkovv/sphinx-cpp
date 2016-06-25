
#pragma once

#include "Logger.h"

#include "HTTPRequest.h"
#include "HTTPResponse.h"

#include <boost/asio.hpp>
#include <string>

#include <memory>
#include <type_traits>

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

public:
  template <typename U = Socket,
            typename = std::enable_if_t<std::is_same<U, TCPSocket>::value>>
  HTTPClient(/*boost::asio::io_service &io_service,*/
             const std::string &address,
             unsigned short port)
    : /*io_service_(io_service),*/
      /*socket_(io_service_),*/
      endpoint_(boost::asio::ip::address::from_string(address), port)
  {
  }

  template <typename U = Socket,
            typename = std::enable_if_t<std::is_same<U, UnixSocket>::value>>
  HTTPClient(/*boost::asio::io_service &io_service,*/
             const std::string &socket_path)
    : /*io_service_(io_service), socket_(io_service_), */ endpoint_(socket_path)
  {
  }

  auto post(const std::string &path, const std::string &data = "")
  {
    io_service_.reset();
    socket_ = std::make_shared<Socket>(io_service_);

    http_request_ = HTTPRequest{HTTPMethod::POST, path, data};

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
                               std::size_t /*length*/)
  {
    if (error_code) {
      logger->error("{0}: {1}", error_code.value(), error_code.message());
      return;
    }

    http_response_ = HTTPResponse{};
    std::istream response_stream(&response_);
    std::string line;
    std::getline(response_stream, line);
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
    std::istream response_stream(&response_);
    std::string headers;
    std::copy_n(std::istreambuf_iterator<char>(response_stream), length,
                std::back_inserter(headers));
    http_response_.parse_headers(headers);

    content_data_left_ =
        get_content_length(http_response_.headers()) - response_.size();

    if (content_data_left_ > 0) {
      async_read_content(content_data_left_);
    }
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
                           std::size_t length)
  {
    bool is_eof = error_code == boost::asio::error::eof;

    if (error_code && !is_eof) {
      logger->error("{0}: {1}", error_code.value(), error_code.message());
      return;
    }

    std::istream response_stream(&response_);
    http_response_.append_data(
        std::string{std::istreambuf_iterator<char>(response_stream), {}});

    content_data_left_ -= length;

    if (!is_eof) {
      async_read_content(content_data_left_);
    }
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

private:
  Logger logger = Sphinx::make_logger("HTTPClient");
};

} // namespace v2
} // namespace Sphinx
} // namespace Docker
