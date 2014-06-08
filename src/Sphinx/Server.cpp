
#include "Server.h"
#include <memory>
#include <stdint.h>
#include <thread>
#include <chrono>

#include <Poco/Net/HTTPServerParams.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/ServerSocket.h>

#include <Poco/SharedPtr.h>

#include "MyRequestHandlerFactory.h"
#include "RootHandler.h"
#include "DataHandler.h"

using Poco::AutoPtr;

namespace Sphinx {


Server::Server() : logger(Poco::Logger::get(name())), terminate(false)
{
    logger.information("Start");
}

Server::~Server()
{
    http_server->stop();
}

void Server::listen()
{
    const uint16_t port = 9999;
    auto params = new Poco::Net::HTTPServerParams();
    params->setMaxQueued(100);
    params->setMaxThreads(16);
    Poco::SharedPtr<MyRequestHandlerFactory> request_handler_factory(new MyRequestHandlerFactory(terminate));
    request_handler_factory->addRequestHandler("/", []() {
        return new RootHandler();
    });
    request_handler_factory->addRequestHandler("/data", []() {
        return new DataHandler();
    });
    Poco::Net::ServerSocket server_socket(port);
    http_server = std::make_unique<Poco::Net::HTTPServer>(request_handler_factory, server_socket, params);
    http_server->start();
}
}

