
#include "HTTPHeaders.h"

#include <regex>

namespace Sphinx {
namespace Docker {
namespace v2 {

using std::experimental::optional;
HTTPHeaders &HTTPHeaders::add_headers(std::string headers)
{
  static const std::regex header_regex("(.+): (.+)\r\n");
  std::smatch sm;

  while (std::regex_search(headers, sm, header_regex)) {
    add_header(sm[1], sm[2].str());
    headers = sm.suffix();
  }

  return *this;
}

optional<std::string> HTTPHeaders::get(const std::string &name) const
{
  auto header_it = find(name);
  if (header_it != end()) {
    return std::experimental::make_optional(header_it->second);
  }
  return std::experimental::optional<std::string>();
}
HTTPHeaders &HTTPHeaders::remove(const std::string &name)
{
  auto header_it = find(name);
  if (header_it != end()) {
    erase(header_it);
  }
  return *this;
}

bool HTTPHeaders::is_chunked() const
{
  auto transfer_encoding_header = get("Transfer-Encoding");

  return transfer_encoding_header &&
         transfer_encoding_header.value() == "chunked";
}

std::size_t HTTPHeaders::get_content_length() const
{
  // TODO(przemkovv): currently the request has to be viable to have this header
  auto content_length_header = get("Content-Length");
  std::size_t content_length = 0;

  if (content_length_header) {
    content_length =
        static_cast<std::size_t>(std::stoi(*content_length_header));
  }
  return content_length;
}

std::string HTTPHeaders::get_content_type() const
{
  auto content_type_header = get("Content-Type");
  std::string content_type;

  if (content_type_header) {
    content_type = *content_type_header;
  }
  return content_type;
}

} // namespace v2
} // namespace Docker
} // namespace Sphinx
