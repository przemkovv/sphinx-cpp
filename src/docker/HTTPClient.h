
#pragma once

#include "TCPConnection.h"

#include "Logger.h"
#include <boost/asio.hpp>
#include <string>

namespace Sphinx {
namespace Docker {

class HTTPClient : protected TCPConnection {
    public:
        HTTPClient(const std::string& address, unsigned short port);

        std::string get(const std::string& path);

    private:
        Logger logger;
};
} // namespace Docker
} // namespace Sphinx
