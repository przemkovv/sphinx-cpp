
#include "RequestHandlerFactory.h"

namespace Sphinx {

RequestHandlerFactory::RequestHandlerFactory() : logger(make_logger(name()))
{
}

void RequestHandlerFactory::addRequestHandler(std::string URI, request_handler_type request_handler)
{
    if (request_handlers.find(URI) != request_handlers.end()) {
        request_handlers.at(URI) = request_handler;
    } else {
        request_handlers.insert(std::make_pair(URI, request_handler));
    }
}

void RequestHandlerFactory::removeRequestHandler(std::string URI)
{
    request_handlers.erase(request_handlers.find(URI));
}

Poco::Net::HTTPRequestHandler *RequestHandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest& request)
{
    auto request_handler_it = request_handlers.find(request.getURI());

    if (request_handler_it != request_handlers.end()) {
        logger->info("Handling requested URI \"" + request.getURI() + "\" from " + request.clientAddress().toString());
        return request_handler_it->second();
    } else {
        logger->info("No  \"" + request.getURI() + "\" URI handler. Requested from "+ request.clientAddress().toString());
        return nullptr;
    }
}
}
