
#include "v1/DockerClient.h"

#include "Logger.h"

#include <fmt/format.h>
#include <boost/asio.hpp>

int main()
{
    spdlog::set_level(spdlog::level::debug);
    auto logger = Sphinx::make_logger("docker");
    Sphinx::make_logger("RESTClient", spdlog::level::debug);
    Sphinx::make_logger("DockerClient", spdlog::level::debug);

    logger->info("Hello");

    try {
        {
            auto docker = Sphinx::Docker::v1::make_docker_client("/var/run/docker.sock");
            //logger->info("Info:\n{} ", docker.getInfo());
            //logger->info("Containers:\n{} ", docker.getContainers());
            logger->info("Containers:\n{} ", docker.getContainers());

            docker.run();
            //logger->info("Images:\n{} ", docker.getImages());
        }
    } catch (std::exception& ex) {
        logger->error("{}", ex.what());
    }

    return 0;
}
