
#pragma once

#include "Sandbox.h"

namespace Sphinx::Executors {

struct CommandOutput {
  std::string stdin;
  std::string stdout;
  std::string stderr;
  int exit_code;
};

class Executor {
private:
  const Sphinx::Sandbox sandbox_;

public:
  Executor(const Sandbox &sandbox) : sandbox_(sandbox) {}

  virtual CommandOutput run_sync(std::string stdin = "") = 0;
  virtual void run_async() {}

  virtual ~Executor() {}
};

} // namespace Sphinx::Executors
