
#pragma once

#include "Compilers/Compiler.h"
#include "Logger.h"

#include <crow.h>

#include <memory>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#pragma clang diagnostic ignored "-Wdocumentation"
#pragma clang diagnostic ignored "-Wswitch-enum"
#pragma clang diagnostic ignored "-Wcovered-switch-default"

#include "json.hpp"

#pragma clang diagnostic pop

namespace Sphinx {
namespace Net {

class Server {
public:
  Server(uint16_t port);
  ~Server();
  const char *name() { return "Sphinx::Server"; }

  void listen();

  void set_compiler(std::unique_ptr<Compilers::Compiler> compiler)
  {
    compiler_ = std::move(compiler);
  }

protected:
  crow::response list_solutions();
  crow::response create_solution(const std::string &body);
  crow::response get_solution(const std::string &solution_id);
  crow::response delete_solution(const std::string &solution_id);
  crow::response get_compilation_result(const std::string &solution_id);
  crow::response list_testcases();
  crow::response create_testcase(const std::string &body);
  crow::response get_testcase(const std::string &testcase_id);
  crow::response list_tests();
  crow::response create_test(const std::string &body);
  crow::response get_test(const std::string &test_id);

  auto make_response(int code, const nlohmann::json &body)
  {
    crow::response response(code, body.dump());
    response.set_header("Content-Type", "application/json");
    return response;
  }

private:
  /* data */
  Logger logger_;

  crow::App<> app_;

  std::vector<Sandbox> samples_;

  const uint16_t port_;

  std::unique_ptr<Compilers::Compiler> compiler_;
};
}
}
