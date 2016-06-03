
#pragma once

#include <Poco/Net/HTTPServer.h>
#include <memory>

#include "RequestHandlerFactory.h"
#include "Logger.h"

namespace Sphinx {
namespace Net {

class Server {
    public:
        Server();
        ~Server();
        const char *name()
        {
            return "Sphinx::Server";
        }

        void listen();

    protected:
        void addRequestHandlers(RequestHandlerFactory *request_handler_factory);

    private:
        /* data */
        Logger logger;
        std::unique_ptr<Poco::Net::HTTPServer> http_server;
};

}
}
