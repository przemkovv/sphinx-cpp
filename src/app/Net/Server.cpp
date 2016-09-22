
#include "SampleData.h"
#include "Server.h"
#include <chrono>
#include <memory>
#include <stdint.h>
#include <thread>

namespace Sphinx {
namespace Net {

Server::Server()
  : logger_(make_logger(name())),
    samples_{SampleData::simple_hello_world(),
             SampleData::simple_hello_world_compile_error(),
             SampleData::simple_echo_out_and_err()}
{
  logger_->info("Starting server");
}

Server::~Server()
{
  logger_->info("Stopping..");
  logger_->info("Stopped");
}

void Server::listen()
{
  CROW_ROUTE(app_, "/")([]() { return "Hello World"; });
  CROW_ROUTE(app_, "/compile/<int>")
  ([this](std::size_t id) {
    compiler_->compile(samples_[id]);

    return compiler_->get_errors();

  });

  const uint16_t port = 9999;

  app_.port(port).multithreaded().run();
}
}
}
