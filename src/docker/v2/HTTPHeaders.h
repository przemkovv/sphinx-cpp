
#pragma once

#include "HTTPCommon.h"
#include <fmt/format.h>

#include <map>
#include <regex>
#include <string>

#include <experimental/optional>

namespace Sphinx {
namespace Docker {
namespace v2 {

class HTTPHeaders : public std::map<std::string, std::string> {
public:
  using std::map<std::string, std::string>::map;

  HTTPHeaders &add_headers(std::string headers)
  {
    static const std::regex header_regex("(.+): (.+)\r\n");
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

  auto get(const std::string &name) const {
    auto header_it = find(name);
    if (header_it != end()) {
      return std::experimental::make_optional(header_it->second);
    }
    return std::experimental::optional<std::string>();

  }

};

} // namespace v2
} // namespace Docker
} // namespace Sphinx
