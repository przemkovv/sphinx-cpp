
#pragma once

#include "Logger.h"

#include "HTTPCommon.h"
#include "HTTPHeaders.h"

#include <map>
#include <string>

namespace Sphinx {
namespace Docker {
namespace v2 {

class HTTPResponse {
private:
  std::string http_version_;
  HTTPStatus status_;
  std::string status_message_;
  HTTPHeaders headers_;
  std::string data_;

public:
  void parse_status_line(const std::string &status_line);
  void parse_headers(const std::string &headers);

  std::string dump() const;
  std::string to_string(bool include_data = false) const;

  template <typename T> void append_data(const T &data)
  {
    data_.append(std::begin(data), std::end(data));
  }

  auto &headers() { return headers_; }
  const auto &headers() const { return headers_; }
  const auto &data() const { return data_; }
  const auto &status() const { return status_; }
  const auto &status_message() const { return status_message_; }
  const auto &http_version() const { return http_version_; }
};

} // namespace v2
} // namespace Docker
} // namespace Sphinx
