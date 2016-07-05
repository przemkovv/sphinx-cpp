
#pragma once

#include "Compiler.h"
#include "File.h"
#include "Logger.h"
#include <string>

namespace Sphinx {
namespace Compilers {

class GXXCompiler : public Compiler {
public:
  GXXCompiler(std::string executable_path);
  std::string get_version() override;

  bool compile(File file) override;
  bool compile(Sandbox sandbox) override;

  virtual const char *name() const override
  {
    return "Sphinx::Compilers::GXXCompiler";
  }

private:
  Logger logger_;

protected:
  virtual Logger &logger() override { return logger_; }
};
}
}
