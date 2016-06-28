
#include "HTTPClient.h"

#include <algorithm>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/endian/conversion.hpp>
#include <typeinfo>

namespace Sphinx {
namespace Docker {
namespace v2 {

template <typename T>
HTTPResponse HTTPClient<T>::request(HTTPMethod method,
                                    const std::string &path,
                                    const std::string &data,
                                    const HTTPHeaders &headers)
{
  prepare_request({method, path, data, headers});
  prepare_response();
  async_connect();

  io_service_.run();
  return http_response_;
}

template <typename T>
HTTPResponse HTTPClient<T>::post(const std::string &path,
                                 const std::string &data,
                                 const HTTPHeaders &headers)
{
  return request(HTTPMethod::POST, path, data, headers);
}

template <typename T>
HTTPResponse HTTPClient<T>::get(const std::string &path,
                                const std::string &data,
                                const HTTPHeaders &headers)
{
  return request(HTTPMethod::GET, path, data, headers);
}

template <typename T>
HTTPResponse HTTPClient<T>::put(const std::string &path,
                                const std::string &data,
                                const HTTPHeaders &headers)
{
  return request(HTTPMethod::PUT, path, data, headers);
}

template <typename T> void HTTPClient<T>::prepare_response()
{
  http_response_ = HTTPResponse{};

  // response_.consume(response_.size());
  // assert(response_.size() == 0);
  if (response_buffer_.size() > 0) {
    logger_response->notice("There is some data in the response_buffer_.");
  }
}

template <typename T>
void HTTPClient<T>::prepare_request(const HTTPRequest &request)
{
  http_request_ = request;

  assert(request_buffer_.size() == 0);
  std::ostream request_stream(&request_buffer_);
  request_stream << http_request_.to_string();
  logger_request->trace(">>{}<<", http_request_.to_string());
}

template <typename T> void HTTPClient<T>::async_connect()
{
  io_service_.reset();
  socket_ = std::make_shared<Socket>(io_service_);

  auto self = shared_from_this();
  socket_->async_connect(endpoint_, [this, self](auto &error_code) {
    handle_connect(error_code);

  });
}

template <typename T>
void HTTPClient<T>::handle_connect(const boost::system::error_code &error_code)
{
  if (error_code) {
    logger->error("{0}: {1}", error_code.value(), error_code.message());
    return;
  }

  auto self = shared_from_this();
  boost::asio::async_write(*socket_, request_buffer_,
                           [this, self](auto &error_code, auto length) {
                             handle_write_request(error_code, length);

                           });
}

template <typename T>
void HTTPClient<T>::handle_write_request(
    const boost::system::error_code &error_code, std::size_t /*length*/)
{
  if (error_code) {
    logger->error("{0}: {1}", error_code.value(), error_code.message());
    return;
  }

  auto self = shared_from_this();
  boost::asio::async_read_until(*socket_, response_buffer_, "\r\n",
                                [this, self](auto &error_code, auto length) {
                                  handle_read_status_line(error_code, length);

                                });
}

template <typename T>
void HTTPClient<T>::handle_read_status_line(
    const boost::system::error_code &error_code, std::size_t length)
{
  if (error_code) {
    logger->error("{0}: {1}", error_code.value(), error_code.message());
    return;
  }

  std::string line = get_n_from_response_stream(length);
  http_response_.parse_status_line(line);

  auto self = shared_from_this();
  boost::asio::async_read_until(*socket_, response_buffer_, "\r\n\r\n",
                                [this, self](auto &error_code, auto length) {
                                  handle_read_headers(error_code, length);

                                });
}

template <typename T>
void HTTPClient<T>::handle_read_headers(
    const boost::system::error_code &error_code, std::size_t length)
{
  if (error_code) {
    logger->error("{0}: {1}", error_code.value(), error_code.message());
    return;
  }
  std::string headers = get_n_from_response_stream(length);
  http_response_.parse_headers(headers);

  auto content_type = http_response_.headers().get_content_type();
  if (boost::starts_with(content_type, "application/json") ||
      boost::starts_with(content_type, "text/plain")) {
    // if (content_type == "application/json") {
    receive_application_json();
  }
  else if (boost::starts_with(content_type,
                              "application/vnd.docker.raw-stream")) {
    // else if (content_type == "application/vnd.docker.raw-stream") {
    logger_response->trace(">>{}<<", http_response_.to_string());
    receive_application_docker_raw_stream();
  }
}

template <typename T>
void HTTPClient<T>::receive_application_docker_raw_stream()
{
  logger->trace("receive_application_docker_raw_stream");
  async_read_docker_raw_stream_header();
}

template <typename T> void HTTPClient<T>::async_read_docker_raw_stream_header()
{
  logger->trace("async_read_docker_raw_stream_header");
  const std::size_t header_length = 8;
  auto self = shared_from_this();
  boost::asio::async_read(
      *socket_, response_buffer_, boost::asio::transfer_exactly(header_length),
      [this, self](auto &error_code, auto length) {
        handle_read_docker_raw_stream_header(error_code, length);
      });
}

template <typename T>
void HTTPClient<T>::handle_read_docker_raw_stream_header(
    const boost::system::error_code &error_code, std::size_t /*length*/)
{
  logger->trace("handle_read_docker_raw_stream_header");
  bool is_eof = error_code == boost::asio::error::eof;

  const std::size_t header_length = 8;
  if (error_code && !is_eof) {
    logger->error("{0}: {1}", error_code.value(), error_code.message());
    return;
  }
  if (is_eof && response_buffer_.size() == 0) {
    return;
  }
  auto header = get_n_from_response_stream(header_length);
  logger->trace("Stream header: {} -- {:x} {:x} {:x} {:x} {:x} {:x} {:x} {:x}",
                header, header[0], header[1], header[2], header[3], header[4],
                header[5], header[6], header[7]);
  auto stream_type = static_cast<StreamType>(header[0]);
  auto data_size = boost::endian::big_to_native(
      *reinterpret_cast<std::uint32_t *>(&header[4]));

  logger->trace("Size of the data: {}", data_size);

  async_read_docker_raw_stream_data(stream_type, data_size);
}

template <typename T>
void HTTPClient<T>::async_read_docker_raw_stream_data(
    const StreamType &stream_type, std::size_t data_size)
{
  logger->trace("async_read_docker_raw_stream_data({}, {})",
                static_cast<uint32_t>(stream_type), data_size);
  auto self = shared_from_this();
  boost::asio::async_read(
      *socket_, response_buffer_, boost::asio::transfer_exactly(data_size),
      [this, self, stream_type, data_size](auto &error_code, auto length) {
        handle_read_docker_raw_stream_data(error_code, length, stream_type,
                                           data_size);
      });
}

template <typename T>
void HTTPClient<T>::handle_read_docker_raw_stream_data(
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
  logger_response->trace(">>{0}<<", data);

  if (!is_eof || response_buffer_.size() != 0) {
    async_read_docker_raw_stream_header();
  }
}

template <typename T> void HTTPClient<T>::receive_application_json()
{
  if (http_response_.headers().is_chunked()) {
    content_length_ = 0;
    async_read_chunk_begin();
  }
  else {
    content_length_ = http_response_.headers().get_content_length();
    content_data_left_ = content_length_ - response_buffer_.size();
    async_read_content(content_data_left_);
  }
}

template <typename T> void HTTPClient<T>::async_read_chunk_begin()
{
  auto self = shared_from_this();
  boost::asio::async_read_until(*socket_, response_buffer_, "\r\n",
                                [this, self](auto &error_code, auto length) {
                                  handle_read_chunk_begin(error_code, length);
                                });
}

template <typename T>
void HTTPClient<T>::handle_read_chunk_begin(
    const boost::system::error_code &error_code, std::size_t length)
{
  if (error_code) {
    logger->error("{0}: {1}", error_code.value(), error_code.message());
    return;
  }
  auto data = get_n_from_response_stream(length);
  chunk_size_ = static_cast<std::size_t>(std::stoi(data, nullptr, 16));

  if (chunk_size_ + 2 >=
      response_buffer_.size()) { // data in the chunk ends with CRLF
    chunk_data_left_ = chunk_size_ + 2 - response_buffer_.size();
    async_read_chunk_data(chunk_data_left_);
  }
  else {
    http_response_.headers()
        .remove("Transfer-Encoding")
        .add_header("Content-Length", content_length_);
  }
}

template <typename T>
void HTTPClient<T>::async_read_chunk_data(std::size_t length)
{
  auto self = shared_from_this();
  boost::asio::async_read(*socket_, response_buffer_,
                          boost::asio::transfer_at_least(length),
                          [this, self](auto &error_code, auto length) {
                            handle_read_chunk_data(error_code, length);
                          });
}

template <typename T>
void HTTPClient<T>::handle_read_chunk_data(
    const boost::system::error_code &error_code, std::size_t /*length*/)
{
  bool is_eof = error_code == boost::asio::error::eof;

  if (error_code && !is_eof) {
    logger->error("{0}: {1}", error_code.value(), error_code.message());
    return;
  }
  if (chunk_size_ + 2 >
      response_buffer_.size()) { // data in the chunk ends with CRLF
    chunk_data_left_ = chunk_size_ + 2 - response_buffer_.size();
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

template <typename T>
auto HTTPClient<T>::get_n_from_response_stream(std::size_t n)
{
  assert(response_buffer_.size() >= n);
  std::istream response_stream(&response_buffer_);
  std::string data;
  data.reserve(n);
  if (n > 0) {
    std::copy_n(std::istreambuf_iterator<char>(response_stream), n,
                std::back_inserter(data));
    response_stream.ignore(1); // discard the last element since copy_n wont
                               // increase the iterator after reading the last
                               // one
  }
  return data;
}

template <typename T>
void HTTPClient<T>::async_read_content(std::size_t bytes_to_read)
{
  auto self = shared_from_this();
  boost::asio::async_read(*socket_, response_buffer_,
                          boost::asio::transfer_at_least(bytes_to_read),
                          [this, self](auto &error_code, auto length) {
                            handle_read_content(error_code, length);
                          });
}

template <typename T>
void HTTPClient<T>::handle_read_content(
    const boost::system::error_code &error_code, std::size_t /*length*/)
{
  bool is_eof = error_code == boost::asio::error::eof;

  if (error_code && !is_eof) {
    logger->error("{0}: {1}", error_code.value(), error_code.message());
    return;
  }

  if (response_buffer_.size() >= content_length_) {
    http_response_.append_data(get_n_from_response_stream(content_length_));
    content_data_left_ = 0;
  }
  else {
    content_data_left_ = content_length_ - response_buffer_.size();
    async_read_content(content_data_left_);
  }
}

template class HTTPClient<TCPSocket>;
template class HTTPClient<UnixSocket>;

} // namespace v2
} // namespace Docker
} // namespace Sphinx
