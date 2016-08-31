#pragma once

#include "Executor.h"

#include "Docker.h"
#include "Sandbox.h"

#include "Logger.h"

#include <string>

namespace Sphinx::Executors {

class DockerExecutor : public Executor {

public:
  DockerExecutor(const Sandbox &sandbox, const std::string &docker_image)
    : Executor(sandbox),
      logger_(make_logger(name())),
      docker_client_(Sphinx::Docker::make_docker_client("/var/run/docker.sock",
                                                        docker_image)),
      io_streams_(io_buffers_.input, io_buffers_.output, io_buffers_.error)
  {
  }

  const char *name() const { return "Sphinx::Executors::DockerExecutor"; }
  virtual ExitCode run(const std::vector<std::string> &args ) override;

  virtual IOStreams& get_io_streams() override;
  virtual void set_timeout(int time) override;

private:
  Logger logger_;
  Logger &logger() { return logger_; }

  std::unique_ptr<Sphinx::Docker::DockerClient> docker_client_;

  Sphinx::Docker::v2::IOBuffers io_buffers_;
  IOStreams io_streams_;



};
} // namespace Sphinx::Executors
