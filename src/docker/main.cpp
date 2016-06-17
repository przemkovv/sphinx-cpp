
#include "RESTClient.h"
#include "UnixSocketConnection.h"

#include "Logger.h"

#include <fmt/format.h>
#include <boost/asio.hpp>

int main()
{
    spdlog::set_level(spdlog::level::debug);
    auto logger = Sphinx::make_logger("docker");
    logger->info("Hello");

    try {
        Sphinx::Docker::RESTClient<Sphinx::Docker::TCPSocket> connection {"127.0.0.1", 2375};
        auto response = connection.get("/info");
        logger->info("Response received:\n{} ", response);
    } catch (std::exception& ex) {
        logger->error("{}", ex.what());
    }

    try {
        Sphinx::Docker::RESTClient<Sphinx::Docker::UnixSocket> connection {"/var/run/docker.sock"};
        auto response = connection.get("/info");
        logger->info("Response received:\n{} ", response);
    } catch (std::exception& ex) {
        logger->error("{}", ex.what());
    }

    return 0;
}
