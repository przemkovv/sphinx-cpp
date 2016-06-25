
#pragma once

#include "Logger.h"

#include "HTTPCommon.h"
#include "HTTPHeaders.h"
#include <fmt/format.h>

#include <map>
#include <regex>
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
  void parse_status_line(const std::string &status_line)
  {

    static const std::regex status_regex("^(HTTP/\\d\\.\\d) (\\d{3}) (.+)");
    std::smatch sm;

    if (std::regex_search(status_line, sm, status_regex)) {
      http_version_ = sm[1];
      status_ = static_cast<HTTPStatus>(std::stoi(sm[2]));
      status_message_ = sm[3];
    }
  }
  void parse_headers(const std::string &headers)
  {
    headers_.add_headers(headers);
  }

  std::string to_string() const
  {
    fmt::MemoryWriter w;
    w.write("Statusline: {0} {1} {2}\n", http_version_,
            static_cast<int>(status_), status_message_);
    w.write("Headers: \n");

    for (auto header : headers_) {
      w.write("{0}: {1}\r\n", header.first, header.second);
    }

    w.write("Data: >>>{0}<<<\n", data_);
    return w.str();
  }

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
