
#include "HTTPResponse.h"

#include <regex>
#include <fmt/format.h>

namespace Sphinx {
namespace Docker {
namespace v2 {

void HTTPResponse::parse_status_line(const std::string &status_line)
{

  static const std::regex status_regex("^(HTTP/\\d\\.\\d) (\\d{3}) (.+)");
  std::smatch sm;

  if (std::regex_search(status_line, sm, status_regex)) {
    http_version_ = sm[1];
    status_ = static_cast<HTTPStatus>(std::stoi(sm[2]));
    status_message_ = sm[3];
  }
}
void HTTPResponse::parse_headers(const std::string &headers)
{
  headers_.add_headers(headers);
}

std::string HTTPResponse::dump() const
{
  fmt::MemoryWriter w;
  w.write("Statusline: {0} {1} {2}\n", http_version_, static_cast<int>(status_),
          status_message_);
  w.write("Headers: \n");

  for (auto header : headers_) {
    w.write("{0}: {1}\r\n", header.first, header.second);
  }

  w.write("Data: >>>{0}<<<\n", data_);
  return w.str();
}

std::string HTTPResponse::to_string(bool include_data) const
{
  fmt::MemoryWriter w;
  w.write("{0} {1} {2}\n", http_version_, static_cast<int>(status_),
          status_message_);

  for (auto header : headers_) {
    w.write("{0}: {1}\r\n", header.first, header.second);
  }
  w.write("\r\n");
  if (include_data && !data_.empty()) {
    w.write("{0}", data_);
  }

  return w.str();
}

} // namespace v2
} // namespace Docker
} // namespace Sphinx
