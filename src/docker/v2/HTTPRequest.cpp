
#include "HTTPRequest.h"

namespace Sphinx {
namespace Docker {
namespace v2 {

void HTTPRequest::addDefaultHeaders()
{
  headers_.add_header("Host", "localhost")
      .add_header("Accept", "*/*")
      .add_header("Connection", "close");
}

HTTPRequest::HTTPRequest(HTTPMethod method,
                         const std::string &uri,
                         const std::string &data,
                         const HTTPHeaders &additional_headers)
  : method_(method), uri_(uri), data_(data)
{
  addDefaultHeaders();
  if (data.size() > 0) {
    headers_.add_header("Content-Length", data.size())
        .add_header("Content-Type", "application/json");
  }
  headers_.add_headers(additional_headers);
}

std::string HTTPRequest::to_string() const
{
  fmt::MemoryWriter w;

  switch (method_) {
  case HTTPMethod::GET:
    w.write("GET");
    break;
  case HTTPMethod::POST:
    w.write("POST");
    break;
  case HTTPMethod::PUT:
    w.write("PUT");
    break;
  case HTTPMethod::DELETE:
    w.write("DELETE");
    break;
  }
  w.write(" {0} HTTP/1.1\r\n", uri_);

  for (auto header : headers_) {
    w.write("{0}: {1}\r\n", header.first, header.second);
  }
  w.write("\r\n");

  if (!data_.empty()) {
    w.write("{0}", data_);
  }

  return w.str();
}

} // namespace v2
} // namespace Docker
} // namespace Sphinx
