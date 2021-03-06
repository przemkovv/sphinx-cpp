
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

  finished_ = false;
  io_service_.run();
  finished_ = true;
  if (writing_thread_.joinable()) {
    writing_thread_.join();
  }
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
    logger_response->warn("There is some data in the response_buffer_.");
  }
}

template <typename T>
void HTTPClient<T>::log_error(const boost::system::error_code &error_code)
{
  logger->error("{0}: {1}", error_code.value(), error_code.message());
}

template <typename T>
void HTTPClient<T>::write_raw_data(boost::asio::streambuf &data)
{

  logger->trace("Write raw data: {} bytes", data.size());
  auto self = shared_from_this();
  boost::asio::async_write(*socket_, data,
                           [this, self](auto &error_code, auto length) {
                             handle_write_raw_data(error_code, length);
                           });
}

template <typename T>
void HTTPClient<T>::handle_write_raw_data(
    const boost::system::error_code &error_code, std::size_t length)
{
  if (error_code) {
    log_error(error_code);
    return;
  }
  logger->debug("Send {} bytes of data to socket.", length);
  writing_data_finished_ = true;
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
    log_error(error_code);
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
    log_error(error_code);
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
    log_error(error_code);
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
    log_error(error_code);
    return;
  }
  std::string headers = get_n_from_response_stream(length);
  http_response_.parse_headers(headers);

  logger_response->trace(">>{0}<<", http_response_.to_string());

  auto content_type = http_response_.headers().get_content_type();
  if (boost::starts_with(content_type, "application/json") ||
      boost::starts_with(content_type, "text/plain")) {
    receive_text();
  }
  else if (boost::starts_with(content_type,
                              "application/vnd.docker.raw-stream")) {
    if (use_input_stream_) {
      start_writing_raw_data_thread();
    }
    receive_application_docker_raw_stream();
  }
}

template <typename T> void HTTPClient<T>::start_writing_raw_data_thread()
{
  writing_data_finished_ = true;
  writing_thread_ = std::thread([this]() {
    auto &data_stream = *get_stream(StreamType::stdin);
    while (!finished_) {
      if (writing_data_finished_ && data_stream.size() > 0) {
        writing_data_finished_ = false;
        this->write_raw_data(data_stream);
      }
      else {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }
    }
  });
  logger->trace("Writing thread has been started.");
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
    log_error(error_code);
    return;
  }
  if (is_eof && response_buffer_.size() == 0) {
    return;
  }
  auto header = get_n_from_response_stream(header_length);
  logger->trace("Stream header: -- {:x} {:x} {:x} {:x} {:x} {:x} {:x} {:x}",
                header[0], header[1], header[2], header[3], header[4],
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
    const StreamType &stream_type,
    std::size_t data_size)
{

  logger->trace("handle_read_docker_raw_stream_data");
  bool is_eof = error_code == boost::asio::error::eof;

  if (error_code && !is_eof) {
    log_error(error_code);
    return;
  }

  if (use_output_streams_) {
    forward_data_to_stream(data_size, stream_type);
  }
  else {
    forward_data_to_response_data(data_size);
  }

  if (!is_eof || response_buffer_.size() != 0) {
    receive_application_docker_raw_stream();
  }
}

template <typename T> void HTTPClient<T>::receive_text()
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
  bool is_eof = error_code == boost::asio::error::eof;

  if (is_eof) {
    assert(response_buffer_.size() == 0);
    return;
  }
  else if (error_code) {
    log_error(error_code);
    return;
  }
  auto data = get_n_from_response_stream(length);
  chunk_size_ = static_cast<std::size_t>(std::stoi(data, nullptr, 16));

  if (chunk_size_ + CRLF >=
      response_buffer_.size()) { // data in the chunk ends with CRLF
    chunk_data_left_ = chunk_size_ + CRLF - response_buffer_.size();
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
    log_error(error_code);
    return;
  }
  if (chunk_size_ + CRLF >
      response_buffer_.size()) { // data in the chunk ends with CRLF
    chunk_data_left_ = chunk_size_ + CRLF - response_buffer_.size();
    async_read_chunk_data(chunk_data_left_);
  }
  else {
    content_length_ += chunk_size_;
    if (use_output_streams_) {
      forward_data_to_stream(chunk_size_, StreamType::stdout);
    }
    else {
      forward_data_to_response_data(chunk_size_);
    }
    get_n_from_response_stream(CRLF); // consume CRLF

    async_read_chunk_begin();
  }
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
    log_error(error_code);
    return;
  }

  if (response_buffer_.size() >= content_length_) {
    if (use_output_streams_) {
      forward_data_to_stream(content_length_, StreamType::stdout);
    }
    else {
      forward_data_to_response_data(content_length_);
    }
    content_data_left_ = 0;
  }
  else {
    content_data_left_ = content_length_ - response_buffer_.size();
    async_read_content(content_data_left_);
  }
}

template <typename T>
void HTTPClient<T>::forward_data_to_response_data(const std::size_t &n)
{
  auto data = get_n_from_response_stream(n);
  http_response_.append_data(data);
}

template <typename T>
void HTTPClient<T>::forward_data_to_stream(const std::size_t &n,
                                           const StreamType &stream_type)
{
  assert(response_buffer_.size() >= n);
  if (n > 0) {
    std::istream response_stream(&response_buffer_);
    std::ostream output_stream(get_stream(stream_type));
    std::copy_n(std::istreambuf_iterator<char>(response_stream), n,
                std::ostream_iterator<char>(output_stream));

    response_stream.ignore(1);
  }
}
template <typename T>
auto HTTPClient<T>::get_n_from_response_stream(std::size_t n)
{
  assert(response_buffer_.size() >= n);
  std::string data;
  data.reserve(n);
  if (n > 0) {
    std::istream response_stream(&response_buffer_);
    std::copy_n(std::istreambuf_iterator<char>(response_stream), n,
                std::back_inserter(data));

    // discard the last element since copy_n wont increase the iterator
    // after
    // reading the last one (see
    // http://cplusplus.github.io/LWG/lwg-active.html#2471 )
    response_stream.ignore(1);
  }
  return data;
}

template class HTTPClient<TCPSocket>;
template class HTTPClient<UnixSocket>;

} // namespace v2
} // namespace Docker
} // namespace Sphinx
