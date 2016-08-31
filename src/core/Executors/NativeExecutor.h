
#pragma once

#include "Executor.h"
#include "process.h"

#include "Sandbox.h"

#include "Logger.h"

#include <string>

namespace Sphinx::Executors {

class NativeExecutor : public Executor {

public:
  NativeExecutor(const Sandbox &sandbox)
    : Executor(sandbox),
      command_(sandbox_.project_executable_path().string(),
               sandbox_.project_root_path().string()),
      logger_(make_logger(name())),
      io_streams_(command_.input_buffer(),
                  command_.output_buffer(),
                  command_.error_buffer())
  {
  }

  const char *name() const { return "Sphinx::Executors::NativeExecutor"; }
  virtual ExitCode run(const std::vector<std::string> &args) override;

  virtual IOStreams &get_io_streams() override;

  virtual void set_timeout(int time) override;

private:
  procxx::process command_;
  Logger logger_;
  Logger &logger() { return logger_; }
  IOStreams io_streams_;
};
} // namespace Sphinx::Executors
