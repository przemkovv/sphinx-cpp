
#pragma once

#include "HTTPCommon.h"
#include <fmt/format.h>

#include <map>
#include <string>

#include <experimental/optional>

namespace Sphinx {
namespace Docker {
namespace v2 {

class HTTPHeaders : public std::map<std::string, std::string> {
public:
  using std::map<std::string, std::string>::map;

  HTTPHeaders &add_headers(std::string headers);

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

  std::experimental::optional<std::string> get(const std::string &name) const;
  HTTPHeaders &remove(const std::string &name);

  bool is_chunked() const;
  std::size_t get_content_length() const;
  std::string get_content_type() const;
};

} // namespace v2
} // namespace Docker
} // namespace Sphinx
