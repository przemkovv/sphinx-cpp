

#pragma once

#include "Compiler.h"


#include "Docker.h"

#include "File.h"
#include "Logger.h"

#include <memory>
#include <string>

namespace Sphinx {
namespace Compilers {

class DockerGXXCompiler : public Compiler {
public:
  const std::vector<std::string> CXXFlags{"-std=c++14", "-Wall", "-Werror"};

  DockerGXXCompiler(std::string executable_path,
    const std::vector<std::string> &flags);
  std::string get_version() override;

  virtual CompilerOutput run(std::vector<std::string> args,
                             std::string root_path = ".") override;

  bool compile(Sandbox sandbox) override;

  virtual const char *name() const override
  {
    return "Sphinx::Compilers::DockerGXXCompiler";
  }

  const std::string &get_output() const override { return result_.out; }
  const std::string &get_errors() const override { return result_.err; }

private:
  CompilerOutput result_;
  std::unique_ptr<Sphinx::Docker::DockerClient> docker_client_;


  Logger logger_;
protected:
  virtual Logger &logger() override { return logger_; }
};
}
}
