
#include "DockerClient.h"

#include "Logger.h"

#include <fmt/format.h>
#include <boost/asio.hpp>

int main()
{
    spdlog::set_level(spdlog::level::debug);
    auto logger = Sphinx::make_logger("docker");
    logger->info("Hello");

    try {
        {
            auto docker = Sphinx::Docker::make_docker_client("/var/run/docker.sock");
            logger->info("Info:\n{} ", docker.getInfo());
            logger->info("Containers:\n{} ", docker.getContainers());
        }
    } catch (std::exception& ex) {
        logger->error("{}", ex.what());
    }

    return 0;
}
