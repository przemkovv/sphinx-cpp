#pragma once

#include "HTTPCommon.h"
#include "HTTPHeaders.h"

#include <fmt/format.h>

#include <map>
#include <string>

namespace Sphinx {
namespace Docker {
namespace v2 {

class HTTPRequest {
  HTTPMethod method_;
  std::string uri_;
  HTTPHeaders headers_;
  std::string data_;

  void addDefaultHeaders();

public:
  HTTPRequest() : HTTPRequest(HTTPMethod::GET, "/") {}

  HTTPRequest(HTTPMethod method,
              const std::string &uri,
              const std::string &data = "",
              const HTTPHeaders &additional_headers = {});

  std::string to_string() const;
};

} // namespace v2
} // namespace Docker
} // namespace Sphinx
