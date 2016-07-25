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
                                                        docker_image))
  {
  }

  const char *name() const { return "Sphinx::Executors::DockerExecutor"; }
  virtual CommandOutput run_sync(std::string stdin = "") override;

private:
  Logger logger_;
  Logger &logger() { return logger_; }

  std::unique_ptr<Sphinx::Docker::DockerClient> docker_client_;
};
} // namespace Sphinx::Executors
