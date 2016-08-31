
#pragma once

#include "Sandbox.h"

#include <iostream>
#include <string>
#include <vector>

namespace Sphinx::Executors {

struct IOStreams {
  std::ostream stdin;
  std::istream stdout;
  std::istream stderr;

  IOStreams(std::streambuf &in_buf,
            std::streambuf &out_buf,
            std::streambuf &err_buf)
    : stdin(&in_buf), stdout(&out_buf), stderr(&err_buf)
  {
  }
};

using ExitCode = int;

class Executor {
protected:
  const Sphinx::Sandbox &sandbox_;

public:
  Executor(const Sphinx::Sandbox &sandbox) : sandbox_(sandbox) {}

  virtual ExitCode run(const std::vector<std::string> &args) = 0;
  virtual ~Executor() {}
  virtual IOStreams& get_io_streams() = 0;

  virtual void set_timeout(int time) = 0;
};

} // namespace Sphinx::Executors
