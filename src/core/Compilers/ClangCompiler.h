
#pragma once

#include "Compiler.h"
#include "File.h"
#include "Logger.h"

#include <string>
#include <tuple>
#include <vector>

#include <memory>

namespace Sphinx {
namespace Compilers {

class ClangCompiler : public Compiler {
public:
  const std::vector<std::string> CXXFlags{"-std=c++14", "-Wall", "-Werror"};

  ClangCompiler(std::string executable_path,
                const std::vector<std::string> &flags);
  std::string get_version() override;

  bool compile(Sandbox sandbox) override;

  const char *name() const override
  {
    return "Sphinx::Compilers::ClangCompiler";
  }

  const std::string &get_output() const override { return result_.out; }
  const std::string &get_errors() const override { return result_.err; }

private:
  CompilerOutput result_;

  Logger logger_;
  virtual Logger &logger() override { return logger_; }
};
}
}
