#pragma once

#include "Logger.h"

#include "Compilers/Compiler.h"
#include <boost/program_options.hpp>
#include <memory>
#include <string>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#pragma clang diagnostic ignored "-Wdocumentation"
#pragma clang diagnostic ignored "-Wswitch-enum"
#pragma clang diagnostic ignored "-Wcovered-switch-default"

#include "json.hpp"

#pragma clang diagnostic pop

namespace Sphinx {

namespace po = boost::program_options;
using json = nlohmann::json;

class Application {
protected:
  // void handle_help(const std::string& name, const std::string& value) ;
  // void display_help() ;
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

private:
  Logger logger_;

  Logger &logger() { return logger_; }

  const std::vector<std::string> args_;
  po::options_description options_description_cli_;
  po::options_description options_description_config_file_;
  po::variables_map config_cli_;

  json config_;

public:
  Application(const std::vector<std::string> &args);
  int run();
};
}
