
#pragma once

#include "Executor.h"

#include "Sandbox.h"

#include "Logger.h"

#include <string>

namespace Sphinx::Executors {

class NativeExecutor : public Executor {

public:
  NativeExecutor(const Sandbox &sandbox)
    : Executor(sandbox), logger_(make_logger(name()))
  {
  }

  const char *name() const { return "Sphinx::Executors::NativeExecutor"; }
  virtual CommandOutput run_sync(std::string stdin = "") override;

private:
  Logger logger_;
  Logger &logger() { return logger_; }
};
} // namespace Sphinx::Executors
