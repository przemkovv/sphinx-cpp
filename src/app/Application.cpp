
#include "Application.h"
#include "utils.h"

#include "File.h"
#include "SampleData.h"
#include "Sandbox.h"

#include "Compilers/ClangCompiler.h"
#include "Compilers/GXXCompiler.h"
#include "Compilers/DockerGXXCompiler.h"
#include "Compilers/MakeCompiler.h"

#include "Net/Server.h"

#include <iostream>

#include <chrono>
#include <memory>
#include <thread>

namespace Sphinx {

Application::Application(const std::vector<std::string> &args)
  : args_(args),
    options_description_cli_(prepare_options_description_cli()),
    options_description_config_file_(prepare_options_description_config_file()),
    config_cli_(parse_command_line_options(args_))
{

  auto log_level = config_cli_["log_level"].as<int>();
  configure_logger(static_cast<spdlog::level::level_enum>(log_level));

  logger()->debug("Command line arguments: {}", args_);
  logger()->debug("Configuration {}", config_cli_);
}

po::options_description Application::prepare_options_description_cli()
{
  po::options_description desc("Allowed options");
  desc.add_options()("help,I", "Display help information")(
      "client-mode,c", "Client mode")("server-mode,s", "Server mode")(
      "log_level", po::value<int>()->default_value(spdlog::level::info),
      "Logging level")("config", po::value<std::vector<std::string>>(),
                       "Configuration file");
  return desc;
}

po::options_description Application::prepare_options_description_config_file()
{
  po::options_description desc("Configuration file only options");
  desc.add_options()("compilers.clang++.flags",
                     po::value<std::vector<std::string>>(),
                     "Flags for the clang compiler.")(
      "compilers.clang++.path", po::value<std::string>(),
      "Path for the clang++ executable.");

  desc.add_options()("compilers.g++.flags",
                     po::value<std::vector<std::string>>(),
                     "Flags for the g++ compiler.")(
      "compilers.g++.path", po::value<std::string>(),
      "Path for the g++ executable.");
  return desc;
}

po::variables_map
Application::parse_command_line_options(const std::vector<std::string> &args)
{
  po::variables_map vm;
  po::store(
      po::command_line_parser(args).options(options_description_cli_).run(),
      vm);

  if (vm.count("config")) {
    auto config_files = vm["config"].as<std::vector<std::string>>();
    for (auto config_file : config_files) {
      std::ifstream file(config_file);
      const bool allow_unregistered = true;
      auto parsed_options = po::parse_config_file(
          file, options_description_config_file_, allow_unregistered);
      po::store(parsed_options, vm);

      auto parsed_options_cli = po::parse_config_file(
          file, options_description_cli_, allow_unregistered);
      po::store(parsed_options_cli, vm);
    }
  }

  po::notify(vm);
  return vm;
}

void Application::configure_logger(spdlog::level::level_enum log_level)
{
  spdlog::set_level(log_level);
  logger_ = make_logger("Application");
}

void Application::run_server_mode()
{
  logger()->info("I'm a server");
  Net::Server server;
  server.listen();
  using namespace std::chrono_literals;
  while (true) {
    logger()->info("Going sleep...");
    std::this_thread::sleep_for(1000ms);
  }
}

std::unique_ptr<Compilers::Compiler> Application::make_clang_compiler()
{
  if (config_cli_.count("compilers.clang++.path") == 0) {
    logger()->error("Couldn't find the clang++ compiler");
    return {};
  }

  auto clangxx_path = config_cli_.at("compilers.clang++.path").as<std::string>();
  auto clangxx_flags =
      config_cli_.at("compilers.clang++.flags").as<std::vector<std::string>>();
  logger()->trace("Clang++ flags: {}", clangxx_flags);
  return std::make_unique<Compilers::ClangCompiler>(clangxx_path,
                                                    clangxx_flags);
}

std::unique_ptr<Compilers::Compiler> Application::make_gxx_compiler()
{
  auto gxx_path = config_cli_.at("compilers.g++.path").as<std::string>();
  auto gxx_flags =
      config_cli_.at("compilers.g++.flags").as<std::vector<std::string>>();
  logger()->trace("G++ flags: {}", gxx_flags);
  return std::make_unique<Compilers::GXXCompiler>(gxx_path, gxx_flags);
}

std::unique_ptr<Compilers::Compiler> Application::make_docker_gxx_compiler()
{
  auto gxx_path = config_cli_.at("compilers.g++.path").as<std::string>();
  auto gxx_flags =
      config_cli_.at("compilers.g++.flags").as<std::vector<std::string>>();
  logger()->trace("G++ flags: {}", gxx_flags);
  return std::make_unique<Compilers::DockerGXXCompiler>(gxx_path, gxx_flags);
}

void Application::run_client_mode()
{
  logger()->info("I'm a client");

  // auto compiler = make_clang_compiler();
  auto compiler = make_docker_gxx_compiler();
  if (!compiler)
    return;

  logger()->info(compiler->get_version());
  std::vector<Sandbox> samples{SampleData::simple_hello_world(),
                               SampleData::simple_hello_world_compile_error()};

  for (auto &sample : samples) {
    logger()->info(
        "------------------------- SAMPLE -------------------------");
    if (compiler->compile(sample)) {
      logger()->info("Compilation was completed succesfully");
    }
    else {
      logger()->error("Compilation failed.");
      logger()->error(compiler->get_errors());
    }
    logger()->info(
        "----------------------------------------------------------");
  }
}

int Application::run()
{
  if (config_cli_.count("help")) {
    std::cout << options_description_cli_;
    std::cout << options_description_config_file_;
  }
  else if (config_cli_.count("server-mode")) {
    run_server_mode();
  }
  else if (config_cli_.count("client-mode")) {
    // run_client_mode();
  }

  return static_cast<int>(ExitCode::OK);
}
} // namespace Sphinx
