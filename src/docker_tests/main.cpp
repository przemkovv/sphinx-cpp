

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

#include <future>
#include <thread>

int main()
{
  spdlog::set_level(spdlog::level::debug);
  auto logger = Sphinx::make_logger("docker");
  Sphinx::make_logger("RESTClient", spdlog::level::warn);
  Sphinx::make_logger("HTTPClient", spdlog::level::trace);
  Sphinx::make_logger("HTTPClient::request", spdlog::level::trace);
  Sphinx::make_logger("HTTPClient::response", spdlog::level::trace);
  Sphinx::make_logger("DockerClient", spdlog::level::trace);
  logger->info("Hello");

  try {
    {
      std::unique_ptr<Sphinx::Docker::DockerClient> docker =
          Sphinx::Docker::make_docker_client("/var/run/docker.sock",
                                             "sphinx-docker");

      {
        auto result = docker->run_command_in_mounted_dir(
            {"g++", "-oecho", "echo.cpp"},
            boost::filesystem::canonical("../data/test_sandbox"));

        logger->info("{}", result);
      }
      //{
      // auto result = docker->run_command_in_mounted_dir(
      //{"./echo"}, boost::filesystem::canonical("../data/test_sandbox"),
      //"Hello world\na\nb\nc\n");

      // logger->info("{}", result);
      //}

      {
        Sphinx::Docker::IOBuffers io_buffers;

        auto result = std::async(std::launch::async, [&docker, &io_buffers]() {
          return docker->run_command_in_mounted_dir(
              {"./echo"}, boost::filesystem::canonical("../data/test_sandbox"),
              io_buffers);
        });
        result.wait_for(std::chrono::milliseconds(1000));

        {
          std::ostream stdin_stream(&io_buffers.input);

          std::size_t count = 3;
          while (count-- > 0) {
            stdin_stream << std::string(count + 1,
                                        static_cast<char>('a' + count))
                         << '\n';

            using namespace std::chrono_literals;
            result.wait_for(500ms);

            std::string data_error{
                std::istreambuf_iterator<char>(&io_buffers.error), {}};
            std::string data{std::istreambuf_iterator<char>(&io_buffers.output),
                             {}};
            logger->debug("Command output {0}", data);
            logger->debug("Command output (error): {0}", data_error);
          }
        }

        logger->info("{}", result.get());
      }
      // logger->info("Images:\n{} ", docker.getImages());
    }
  }
  catch (std::exception &ex) {
    logger->error("{}", ex.what());
  }

  return 0;
}
