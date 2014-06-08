
#pragma once

#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>
#include "RootHandler.h"
#include "DataHandler.h"
#include <atomic>

class MyRequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
{
public:
    MyRequestHandlerFactory(std::atomic<bool>& terminate) : terminate(terminate) {}
    
    Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request) {
        if (request.getURI() == "/") {
            return new RootHandler();
        }
        else {
            terminate.store(true);
            return new DataHandler();
        }
    }

private:
    std::atomic<bool>& terminate;
    /* data */
};
