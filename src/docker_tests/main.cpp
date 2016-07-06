

#include "Logger.h"

#include "Docker.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#pragma clang diagnostic ignored "-Wdocumentation"
#pragma clang diagnostic ignored "-Wswitch-enum"
#pragma clang diagnostic ignored "-Wcovered-switch-default"

#include "json.hpp"

#pragma clang diagnostic pop

#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <fmt/format.h>

int main()
{
  spdlog::set_level(spdlog::level::debug);
  auto logger = Sphinx::make_logger("docker");
  Sphinx::make_logger("RESTClient", spdlog::level::warn);
  Sphinx::make_logger("HTTPClient", spdlog::level::debug);
  Sphinx::make_logger("HTTPClient::request", spdlog::level::debug);
  Sphinx::make_logger("HTTPClient::response", spdlog::level::debug);
  Sphinx::make_logger("DockerClient", spdlog::level::trace);
  logger->info("Hello");

  try {
    {
      std::unique_ptr<Sphinx::Docker::DockerClient> docker =
          Sphinx::Docker::make_docker_client("/var/run/docker.sock");

      auto result = docker->run_command_in_mounted_dir(
          {"./main"}, boost::filesystem::canonical("../data/test_sandbox"));
          //{"g++", "main.cpp"},
          //boost::filesystem::canonical("../data/test_sandbox"));

      logger->info("{}", result);
      // logger->info("Images:\n{} ", docker.getImages());
    }
  }
  catch (std::exception &ex) {
    logger->error("{}", ex.what());
  }

  return 0;
}
