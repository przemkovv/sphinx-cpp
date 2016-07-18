
#pragma once

#include "File.h"
#include "Logger.h"
#include "Sandbox.h"

#include <string>
#include <tuple>
#include <vector>

namespace Sphinx {
namespace Compilers {

struct CompilerOutput {
  int exit_code;
  std::string out;
  std::string err;
};

class Compiler {
protected:
  std::string executable_path_;
  Compiler(std::string executable_path);

  virtual CompilerOutput run(std::vector<std::string> args,
                             std::string root_path = ".");

  std::string output_filepath_;

  virtual Logger &logger() = 0;

public:
  const std::string &output_filepath() const { return output_filepath_; }
  virtual std::string get_version() = 0;
  void compile();
  virtual bool compile(Sandbox sandbox) = 0;

  virtual const std::string &get_output() const = 0;
  virtual const std::string &get_errors() const = 0;

  virtual const char *name() const { return "Sphinx::Compilers::Compiler"; }

  virtual ~Compiler() {}
};
}
}
