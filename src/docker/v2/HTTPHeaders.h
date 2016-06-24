
#pragma once

#include "HTTPCommon.h"
#include <fmt/format.h>

#include <map>
#include <regex>
#include <string>

namespace Sphinx {
namespace Docker {
namespace v2 {

class HTTPHeaders : public std::map<std::string, std::string> {
public:
  using std::map<std::string, std::string>::map;

  //HTTPHeaders() : std::map<std::string, std::string>::map() {}

  //HTTPHeaders(const std::string &headers) { add_headers(headers); }

  HTTPHeaders &add_headers(std::string headers)
  {
    std::regex header_regex("(.+): (.+)\r\n");
    std::smatch sm;

    while (std::regex_search(headers, sm, header_regex)) {
      add_header(sm[1], sm[2].str());
      headers = sm.suffix();
    }

    return *this;
  }

  template <typename T>
  HTTPHeaders &add_header(const std::string &name, const T &value)
  {
    emplace(std::make_pair(name, fmt::format("{}", value)));
    return *this;
  }

  template <typename T> HTTPHeaders &add_headers(const T &headers)
  {
    insert(std::begin(headers), std::end(headers));
    return *this;
  }
};

} // namespace v2
} // namespace Sphinx
} // namespace Docker
