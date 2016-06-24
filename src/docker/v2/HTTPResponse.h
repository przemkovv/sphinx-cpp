
#pragma once

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
  HTTPStatus status_;
  HTTPHeaders headers_;
  std::string data_;

  HTTPResponse(std::string headers, std::string data)
    : headers_(headers), data_(data)
  {
    std::regex status_regex("^HTTP/\\d\\.\\d (\\d{3}) .+");
    std::smatch sm;

    if (std::regex_search(headers, sm, status_regex)) {
      status_ = static_cast<HTTPStatus>(std::stoi(sm[1]));
    }
  }

  std::string to_string() const
  {
    fmt::MemoryWriter w;
    w.write("Response status: {0}\n", static_cast<int>(status_));
    w.write("Response headers: \n");

    for (auto header : headers_) {
      w.write("{0}: {1}\r\n", header.first, header.second);
    }

    w.write("Response data: {0}\n", data_);
    return w.str();
  }
};

} // namespace v2
} // namespace Sphinx
} // namespace Docker
