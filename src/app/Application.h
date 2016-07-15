#pragma once

#include "Logger.h"

#include "Compilers/Compiler.h"
#include <memory>
#include <boost/program_options.hpp>
#include <string>


namespace Sphinx {

namespace po = boost::program_options;

class Application {
protected:
  // void handle_help(const std::string& name, const std::string& value) ;
  // void display_help() ;
  void configure_logger(spdlog::level::level_enum level);
  void run_client_mode();
  [[noreturn]] void run_server_mode();

  po::options_description prepare_options_description_cli();
  po::options_description prepare_options_description_config_file();
  po::variables_map
  parse_command_line_options(const std::vector<std::string> &arguments);
private:
  std::unique_ptr<Compilers::Compiler> make_clang_compiler();
  std::unique_ptr<Compilers::Compiler> make_gxx_compiler();



private:
  Logger logger_;

  Logger &logger() { return logger_; }

  const std::vector<std::string> args_;
  po::options_description options_description_cli_;
  po::options_description options_description_config_file_;
  po::variables_map config_;

public:
  Application(const std::vector<std::string> &args);
  int run();
};
}
