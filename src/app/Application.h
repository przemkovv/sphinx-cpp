#pragma once

#include "Logger.h"

#include "Compilers/Compiler.h"
#include <boost/program_options.hpp>
#include <memory>
#include <string>

#include <nlohmann/json.hpp>


using json = nlohmann::json;

namespace Sphinx {

namespace po = boost::program_options;

class Application {
protected:
  void configure_logger(spdlog::level::level_enum level);
  void run_client_mode();
  [[noreturn]] void run_server_mode();

  po::options_description prepare_options_description_cli();
  po::variables_map
  parse_command_line_options(const std::vector<std::string> &arguments);
  json parse_config_file();

private:
  std::unique_ptr<Compilers::Compiler> make_clang_compiler();
  std::unique_ptr<Compilers::Compiler> make_gxx_compiler();
  std::unique_ptr<Compilers::Compiler> make_docker_gxx_compiler();
  std::unique_ptr<Compilers::Compiler> make_compiler(std::string name);

private:
  Logger logger_;

  Logger &logger() { return logger_; }

  const std::vector<std::string> args_;
  po::options_description options_description_cli_;
  po::variables_map config_cli_;

  json config_;

public:
  Application(const std::vector<std::string> &args);
  int run();
};
}
