
#include "DockerClient.h"
#include "Logger.h"

#include "v2/DockerClient.h"
#include "v2/HTTPClient.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#pragma clang diagnostic ignored "-Wdocumentation"
#pragma clang diagnostic ignored "-Wswitch-enum"
#pragma clang diagnostic ignored "-Wcovered-switch-default"

#include "json.hpp"

#pragma clang diagnostic pop

#include <boost/asio.hpp>
#include <fmt/format.h>

int main()
{
  spdlog::set_level(spdlog::level::debug);
  auto logger = Sphinx::make_logger("docker");
  Sphinx::make_logger("RESTClient", spdlog::level::warn);
  Sphinx::make_logger("HTTPClient", spdlog::level::trace);
  Sphinx::make_logger("DockerClient", spdlog::level::debug);
  logger->info("Hello");

  try {
    {
        // using Sphinx::Docker::v2::make_http_client;
        // auto http_client = make_http_client("/var/run/docker.sock");
        ////auto http_client = make_http_client("127.0.0.1", 2375);
        //{
        // auto result = http_client->get("/info");
        // logger->info("{}", result.to_string());
        //}
        //{
        // auto result = http_client->get("/containers/json?all=1&size=1");
        // logger->info("{}", result.to_string());
        //}
        //{

        // using json = nlohmann::json;
        // json container = {{"Hostname", ""},
        //{"User", ""},
        //{"Memory", 0},
        //{"MemorySwap", 0},
        //{"AttachStdin", false},
        //{"AttachStdout", true},
        //{"AttachStderr", true},
        //{"Tty", false},
        //{"OpenStdin", false},
        //{"StdinOnce", false},
        //{"Cmd", {"date"}},
        //{"Image", "sphinx-docker"},
        //{
        //"WorkingDir", "",
        //}};
        // const auto request_data = container.dump();
        // auto result = http_client->post("/containers/create", request_data);
        // logger->info("{}", result.to_string());
        // auto response_json = json::parse(result.data());
        // auto container_id = response_json["Id"].get<std::string>();

        // result = http_client->post(
        // fmt::format("/containers/{0}/start", container_id));
        // logger->info("{}", result.to_string());
        //}
    } {

    } {
      auto docker =
          Sphinx::Docker::v2::make_docker_client("/var/run/docker.sock");
      logger->info("Info:\n{} ", docker.getInfo());
      // logger->info("Containers:\n{} ", docker.getContainers());
      // logger->info("Containers:\n{} ", docker.getContainers());
      docker.run();
      // logger->info("Images:\n{} ", docker.getImages());
    }
  }
  catch (std::exception &ex) {
    logger->error("{}", ex.what());
  }

  return 0;
}
