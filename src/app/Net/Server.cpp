
#include "Server.h"
#include <memory>
#include <stdint.h>
#include <thread>
#include <chrono>

#include <Poco/Net/HTTPServerParams.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/ServerSocket.h>

#include <Poco/SharedPtr.h>

#include "RootHandler.h"
#include "DataHandler.h"

using Poco::Net::ServerSocket;
using Poco::SharedPtr;

namespace Sphinx {
namespace Net {

Server::Server() : logger(make_logger(name()))
{
    logger->info("Start");
}

Server::~Server()
{
    logger->info("Stopping..");
    http_server->stop();
    logger->info("Stopped");
}

void Server::addRequestHandlers(RequestHandlerFactory *request_handler_factory)
{
    request_handler_factory->addRequestHandler("/", []() {
        return new RootHandler();
    });
    request_handler_factory->addRequestHandler("/data", []() {
        return new DataHandler();
    });
}

void Server::listen()
{
    const uint16_t port = 9999;
    auto params = new Poco::Net::HTTPServerParams();
    params->setMaxQueued(100);
    params->setMaxThreads(16);
    SharedPtr<RequestHandlerFactory> request_handler_factory(new RequestHandlerFactory());
    addRequestHandlers(request_handler_factory.get());
    ServerSocket server_socket(port);
    http_server = std::make_unique<Poco::Net::HTTPServer>(request_handler_factory, server_socket, params);
    http_server->start();
}

}
}
