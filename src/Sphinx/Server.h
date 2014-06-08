
#pragma once

#include <atomic>
#include <Poco/Logger.h>
#include <Poco/Net/HTTPServer.h>
#include <memory>

namespace Sphinx {
class Server {
    public:
        Server();
        ~Server();
        const char *name()
        {
            return "Sphinx::Server";
        }

        void listen();

    private:
        /* data */
        Poco::Logger& logger;
        std::atomic<bool> terminate;
        std::unique_ptr<Poco::Net::HTTPServer> http_server;
};

}
