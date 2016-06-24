
#include "DockerClient.h"
#include "v2/HTTPClient.h"

#include "Logger.h"

#include <boost/asio.hpp>
#include <fmt/format.h>

int main()
{
  spdlog::set_level(spdlog::level::debug);
  auto logger = Sphinx::make_logger("docker");
  Sphinx::make_logger("RESTClient", spdlog::level::warn);
  Sphinx::make_logger("HTTPClient", spdlog::level::debug);
  Sphinx::make_logger("DockerClient", spdlog::level::debug);
  logger->info("Hello");

  try {
    {
      using namespace Sphinx::Docker::v2;
      boost::asio::io_service io_service;
      auto http_client = std::make_shared<HTTPClient<UnixSocket>>(io_service, "/var/run/docker.sock");
      auto result = http_client->get("/info");
      logger->info("{}", result.to_string());
      //http_client->get("/containers/json?all=1&size=1");
    }
    {
      auto docker = Sphinx::Docker::make_docker_client("/var/run/docker.sock");
      // logger->info("Info:\n{} ", docker.getInfo());
      // logger->info("Containers:\n{} ", docker.getContainers());
      logger->info("Containers:\n{} ", docker.getContainers());
      // docker.run();
      // logger->info("Images:\n{} ", docker.getImages());
    }
  }
  catch (std::exception &ex) {
    logger->error("{}", ex.what());
  }

  return 0;
}
