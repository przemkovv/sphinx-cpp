#include "HTTPClient.h"

namespace Sphinx {
namespace Docker {

HTTPClient::HTTPClient(const std::string& address, unsigned short port)
    : TCPConnection(address, port), logger(make_logger("HTTPConnection"))
{
}

std::string HTTPClient::get(const std::string& path)
{
    auto request = fmt::format("GET {0} HTTP/1.1\r\n"
                               "Host: localhost\r\n"
                               "Accept: */*\r\n"
                               "Connection: close\r\n"
                               "\r\n\r\n",
                               path);
    logger->debug("HTTP Request:\n{}", request);
    send(request);
    auto response = receive();
    logger->debug("HTTP Response:\n{}", response);
    return response;
}

} // namespace Docker
} // namespace Sphinx
