
#pragma once

#include "IOConnection.h"

#include "Logger.h"
#include <fmt/format.h>

#include <boost/asio.hpp>
#include <string>

namespace Sphinx {
namespace Docker {

template <typename T = TCPSocket>
class RESTClient : protected IOConnection<T> {
        using IOConnection<T>::send;
        using IOConnection<T>::receive;
        using IOConnection<T>::reconnect;

    public:
        using IOConnection<T>::IOConnection;

        std::string get(const std::string& path)
        {
            reconnect();
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

    private:
        Logger logger = make_logger("HTTPClient");
};
} // namespace Docker
} // namespace Sphinx
