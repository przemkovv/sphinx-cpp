
#pragma once

#include <map>
#include <functional>
#include <string>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Logger.h>

namespace Sphinx {

class RequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory {
    public:
        typedef std::function<Poco::Net::HTTPRequestHandler*()> request_handler_type;

        RequestHandlerFactory();
        void addRequestHandler(std::string URI, request_handler_type request_handler);
        void removeRequestHandler(std::string URI);
        Poco::Net::HTTPRequestHandler *createRequestHandler(const Poco::Net::HTTPServerRequest& request);

        const char *name()
        {
            return "Sphinx::RequestHandlerFactory";
        }
    private:
        /* data */
        Poco::Logger& logger;
        std::map<std::string, request_handler_type> request_handlers;
};


}
