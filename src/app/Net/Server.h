
#pragma once

#include "Compilers/Compiler.h"
#include "Logger.h"

#include <crow.h>

#include <memory>

namespace Sphinx {
namespace Net {

class Server {
public:
  Server();
  ~Server();
  const char *name() { return "Sphinx::Server"; }

  void listen();

  void set_compiler(std::unique_ptr<Compilers::Compiler> compiler)
  {
    compiler_ = std::move(compiler);
  }

private:
  /* data */
  Logger logger_;

  crow::App<> app_;

  std::vector<Sandbox> samples_;

  std::unique_ptr<Compilers::Compiler> compiler_;
};
}
}
