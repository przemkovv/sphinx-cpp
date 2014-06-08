
#include "Server.h"
#include <memory>
#include <stdint.h>
#include <thread>
#include <chrono>

#include <Poco/Net/HTTPServerParams.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/ServerSocket.h>

#include "MyRequestHandlerFactory.h"

namespace Sphinx{ 


    Server::Server() : logger(Poco::Logger::get(name())), terminate(false) {

        logger.information("Start");
    }

    Server::~Server(){ 
    }

    void Server::listen() {
        const uint16_t port = 9999;
        auto params = new Poco::Net::HTTPServerParams();
        params->setMaxQueued(100);
        params->setMaxThreads(16);

        Poco::Net::ServerSocket server_socket(port);
        Poco::Net::HTTPServer http_server(new MyRequestHandlerFactory(terminate), server_socket, params);


        http_server.start();

        std::chrono::milliseconds duration(1'000);
        std::this_thread::sleep_for(duration);

        //waitForTerminationRequest();
        while( !terminate)  {
            logger.information("Going sleep...");
            std::this_thread::sleep_for(duration);
        }


        http_server.stop();
    }
}
