
#include "Application.h"
#include "utils.h"

#include "File.h"
#include "SampleData.h"
#include "Sandbox.h"

#include "Compilers/ClangCompiler.h"
#include "Compilers/DockerGXXCompiler.h"
#include "Compilers/GXXCompiler.h"
#include "Compilers/MakeCompiler.h"

#include "Executors/Native.h"

#include "Net/Server.h"

#include <iostream>

#include <chrono>
#include <memory>
#include <thread>

namespace Sphinx {

Application::Application(const std::vector<std::string> &args)
  : args_(args),
    options_description_cli_(prepare_options_description_cli()),
    config_cli_(parse_command_line_options(args_)),
    config_(parse_config_file())
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
      "Logging level")("config", po::value<std::string>(),
                       "Configuration file");
  return desc;
}

po::variables_map
Application::parse_command_line_options(const std::vector<std::string> &args)
{
  po::variables_map vm;
  po::store(
      po::command_line_parser(args).options(options_description_cli_).run(),
      vm);
  po::notify(vm);
  return vm;
}

json Application::parse_config_file()
{
  json config;
  if (config_cli_.count("config")) {
    auto config_file = config_cli_["config"].as<std::string>();
    std::ifstream file(config_file);
    config << file;
  }
  return config;
}

void Application::configure_logger(spdlog::level::level_enum log_level)
{
  spdlog::set_level(log_level);
  logger_ = make_logger("Application");

  auto logger_levels = config_["loggers"];
  for (auto it = logger_levels.begin(); it != logger_levels.end(); ++it) {
    logger()->debug("Logger {} has log level {}.", it.key(),
                    it.value().get<int>());
    make_logger(it.key(),
                static_cast<spdlog::level::level_enum>(it.value().get<int>()));
  }
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
  auto clangxx_path = config_["/compilers/clang++/path"_json_pointer];
  if (clangxx_path.is_null()) {
    logger()->error("Couldn't find the clang++ compiler");
    return {};
  }

  auto clangxx_flags = config_["/compilers/clang++/flags"_json_pointer]
                           .get<std::vector<std::string>>();
  logger()->trace("Clang++ flags: {}", clangxx_flags);
  return std::make_unique<Compilers::ClangCompiler>(
      clangxx_path.get<std::string>(), clangxx_flags);
}

std::unique_ptr<Compilers::Compiler> Application::make_gxx_compiler()
{
  auto gxx_path = config_["/compilers/g++/path"_json_pointer];
  if (gxx_path.is_null()) {
    logger()->error("Couldn't find the g++ compiler");
    return {};
  }
  auto gxx_flags = config_["/compilers/g++/flags"_json_pointer]
                       .get<std::vector<std::string>>();
  logger()->trace("G++ flags: {}", gxx_flags);
  return std::make_unique<Compilers::GXXCompiler>(gxx_path.get<std::string>(),
                                                  gxx_flags);
}

std::unique_ptr<Compilers::Compiler> Application::make_docker_gxx_compiler()
{
  auto gxx_path =
      config_["/compilers/docker-g++/path"_json_pointer].get<std::string>();
  auto gxx_flags = config_["/compilers/docker-g++/flags"_json_pointer]
                       .get<std::vector<std::string>>();
  auto docker_image = config_["/compilers/docker-g++/image-name"_json_pointer]
                          .get<std::string>();
  logger()->trace("G++ flags: {}", gxx_flags);
  return std::make_unique<Compilers::DockerGXXCompiler>(gxx_path, gxx_flags,
                                                        docker_image);
}

std::unique_ptr<Compilers::Compiler>
Application::make_compiler(std::string name)
{
  if (name == "clang++") {
    return make_clang_compiler();
  }
  else if (name == "g++") {
    return make_gxx_compiler();
  }
  else if (name == "docker-g++") {
    return make_docker_gxx_compiler();
  }
  return {};
}

std::unique_ptr<Executors::Executor>
Application::make_executor(std::string name, const Sandbox &sandbox)
{
  if (name == "native") {
    return std::make_unique<Executors::Native>(sandbox);
  }
  return {};
}

void Application::run_client_mode()
{
  logger()->info("I'm a client");

  auto default_compiler =
      config_["/compilers/default"_json_pointer].get<std::string>();

  auto default_executor =
      config_["/executors/default"_json_pointer].get<std::string>();

  auto compiler = make_compiler(default_compiler);
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

      logger()->info("Running compiler program");
      auto executor = make_executor(default_executor, sample);
      auto output = executor->run_sync();

      logger()->debug("Program stdout: {}", output.stdout);
      logger()->debug("Program stderr: {}", output.stderr);
      logger()->debug("Program exit code: {}", output.exit_code);
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
  logger()->debug("Configuration file: {}", config_.dump(2));
  if (config_cli_.count("help")) {
    std::cout << options_description_cli_;
  }
  else if (config_cli_.count("server-mode")) {
    run_server_mode();
  }
  else if (config_cli_.count("client-mode")) {
    run_client_mode();
  }

  return static_cast<int>(ExitCode::OK);
}
} // namespace Sphinx
