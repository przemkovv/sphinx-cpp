
#pragma once

#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <atomic>
#include <map>
#include <functional>
#include <string>

class MyRequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory {
    public:
        typedef std::function<Poco::Net::HTTPRequestHandler*()> request_handler_type;

        MyRequestHandlerFactory(std::atomic<bool>& terminate) : terminate(terminate) {}

        void addRequestHandler(std::string URI, request_handler_type request_handler)
        {
            if (request_handlers.find(URI) != request_handlers.end()) {
                request_handlers.at(URI) = request_handler;
            } else {
                request_handlers.insert(std::make_pair(URI, request_handler));
            }
        }

        void removeRequestHandler(std::string URI)
        {
            request_handlers.erase(request_handlers.find(URI));
        }

        Poco::Net::HTTPRequestHandler *createRequestHandler(const Poco::Net::HTTPServerRequest& request)
        {
            auto request_handler_it = request_handlers.find(request.getURI());

            if (request_handler_it != request_handlers.end()) {
                return request_handler_it->second();
            } else {
                return nullptr;
            }
        }

    private:
        std::atomic<bool>& terminate;

        std::map<std::string, request_handler_type> request_handlers;
        /* data */
};



