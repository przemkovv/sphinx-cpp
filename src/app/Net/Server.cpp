
#include "SampleData.h"
#include "Server.h"
#include "sphinx_assert.h"
#include <chrono>
#include <memory>
#include <stdint.h>
#include <thread>

namespace Sphinx {
namespace Net {

Server::Server(uint16_t port)
  : logger_(make_logger(name())),
    samples_{SampleData::simple_hello_world(),
             SampleData::simple_hello_world_compile_error(),
             SampleData::simple_echo_out_and_err()},
    port_(port)
{
  logger_->info("Starting server");
}

Server::~Server()
{
  logger_->info("Stopping..");
  logger_->info("Stopped");
}

crow::response Server::list_solutions() { return make_response(200, {{}}); }
crow::response Server::create_solution(const std::string &/* body */)
{
  return make_response(200, {});
}
crow::response Server::get_solution(const std::string &/* solution_id */)
{
  return make_response(200, {});
}
crow::response Server::delete_solution(const std::string &/* solution_id */)
{
  return make_response(200, {});
}
crow::response Server::get_compilation_result(const std::string &/* solution_id */)
{
  return make_response(200, {});
}
crow::response Server::list_testcases() { return make_response(200, {}); }
crow::response Server::create_testcase(const std::string &/* body */)
{
  return make_response(200, {});
}
crow::response Server::get_testcase(const std::string &/* testcase_id */)
{
  return make_response(200, {});
}
crow::response Server::list_tests() { return make_response(200, {}); }
crow::response Server::create_test(const std::string &/* body */)
{
  return make_response(200, {});
}
crow::response Server::get_test(const std::string &/* test_id */)
{
  return make_response(200, {});
}

void Server::listen()
{
  /*
   * GET /solutions -> 200
   * POST /solutions -> 201
   */
  CROW_ROUTE(app_, "/solutions")
      .methods("GET"_method, "POST"_method)([this](const crow::request &req) {
        if (req.method == "GET"_method)
          return list_solutions();
        else if (req.method == "POST"_method)
          return create_solution(req.body);
        else
          SPHINX_ASSERT(false, "/solutions, method: {}",
                        crow::method_name(req.method));
      });
  /*
   * GET /solutions/{solution_id} -> 200
   * DELETE /solutions/{solution_id} -> 204
   */
  CROW_ROUTE(app_, "/solutions/<string>")
      .methods("GET"_method, "DELETE"_method)(
          [this](const crow::request &req, const std::string &solution_id) {
            if (req.method == "GET"_method)
              return get_solution(solution_id);
            else if (req.method == "DELETE"_method)
              return delete_solution(solution_id);
            else
              SPHINX_ASSERT(false, "/solutions/{}, method: {}", solution_id,
                            crow::method_name(req.method));
          });
  /*
   * GET /solutions/{solution_id}/compiler_output -> 200
   * GET /solutions/{solution_id}/compiler_output -> 304
   */
  CROW_ROUTE(app_, "/solutions/<string>/compiler_output")
      .methods("GET"_method)([this](const std::string &solution_id) {
        return get_compilation_result(solution_id);
      });

  /*
   * GET /testcases -> 200
   * POST /testcases -> 200
   */
  CROW_ROUTE(app_, "/testcases")
      .methods("GET"_method, "POST"_method)([this](const crow::request &req) {
        if (req.method == "GET"_method)
          return list_testcases();
        else if (req.method == "POST"_method)
          return create_testcase(req.body);
        else
          SPHINX_ASSERT(false, "/testcases, method: {}",
                        crow::method_name(req.method));
      });
  /*
   * GET /testcases/{testcase_id} -> 200
   */
  CROW_ROUTE(app_, "/testcases/<string>")
      .methods("GET"_method)([this](const std::string &testcase_id) {
        return get_testcase(testcase_id);
      });
  /*
   * GET /tests -> 200
   * POST /tests -> 202
   */
  CROW_ROUTE(app_, "/tests")
      .methods("GET"_method, "POST"_method)([this](const crow::request &req) {
        if (req.method == "GET"_method)
          return list_tests();
        else if (req.method == "POST"_method)
          return create_test(req.body);
        else
          SPHINX_ASSERT(false, "/tests, method: {}",
                        crow::method_name(req.method));
      });
  /*
   * GET /tests/{test_id} -> 200
   * GET /tests/{test_id} -> 202
   */
  CROW_ROUTE(app_, "/tests/<string>")
      .methods("GET"_method)(
          [this](const std::string &test_id) { return get_test(test_id); });

  app_.port(port_).multithreaded().run();
}
}
}
