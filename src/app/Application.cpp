
#include "Application.h"
#include "utils.h"

#include "File.h"
#include "Sandbox.h"
#include "SampleData.h"

#include "Compilers/MakeCompiler.h"
#include "Compilers/ClangCompiler.h"

#include "Net/Server.h"

#include <iostream>

#include <memory>
#include <thread>
#include <chrono>


namespace Sphinx {

namespace po = boost::program_options;

Application::Application(const std::vector<std::string>& args) :
    args(args),
    options_description(prepare_options_description())
{
    config = parse_command_line_options(args);

    auto log_level = config["log_level"].as<int>();
    configureLogger(static_cast<spdlog::level::level_enum>(log_level));

    logger->debug("Command line arguments: {}", args);
    logger->debug("Configuration {}", config);
}

po::options_description Application::prepare_options_description()
{
    po::options_description desc("Allowed options");
    desc.add_options()
    ("help,I", "Display help information")
    ("client-mode,c", "Client mode")
    ("server-mode,s", "Server mode")
    ("log_level", po::value<int>()->default_value(spdlog::level::info), "Logging level")
    ("config", po::value<std::vector<std::string>>(), "Configuration file");
    desc.add_options()
    ("compilers.clang++.flags", po::value<std::vector<std::string>>(), "Flags for the clang compiler.")
    ("compilers.clang++.path", po::value<std::string>(), "Path for the clang++ executable.");
    return desc;
}


po::variables_map Application::parse_command_line_options(const std::vector<std::string>& args)
{
    po::variables_map vm;
    po::store(po::command_line_parser(args).options(options_description).run(), vm);

    if (vm.count("config")) {
        auto config_files = vm["config"].as<std::vector<std::string>>();
        for (auto config_file : config_files) {
            std::ifstream file(config_file);
            const bool allow_unregistered = false;
            auto parsed_options = po::parse_config_file(file, options_description, allow_unregistered);
            po::store(parsed_options, vm);
        }
    }

    po::notify(vm);
    return vm;
}

void Application::configureLogger(spdlog::level::level_enum log_level)
{
    spdlog::set_level(log_level);
    logger = make_logger("Application");
}

void Application::runServerMode()
{
    logger->info("I'm a server");
    Net::Server server;
    server.listen();
    using namespace std::chrono_literals;
    while (true) {
        logger->info("Going sleep...");
        std::this_thread::sleep_for(1000ms);
    }
}

void Application::runClientMode()
{
    logger->info("I'm a client");

    if (config.count("compilers.clang++.path") == 0) {
        return;
    }

    auto clangxx_path = config.at("compilers.clang++.path").as<std::string>();
    auto clangxx_flags = config.at("compilers.clang++.flags").as<std::vector<std::string>>();
    logger->debug("Clang++ flags: {}", clangxx_flags);
    Compilers::ClangCompiler compiler{clangxx_path, clangxx_flags};
    logger->info(compiler.getVersion());
    auto sample = SampleData::simpleHelloWorld();

    if (compiler.compile(sample)) {
        logger->info("Compilation was completed succesfully");
    } else {
        logger->error("Compilation failed.");
        logger->error(compiler.getErrors());
    }
}

int Application::run()
{
    if (config.count("help")) {
        std::cout << options_description;
    } else if (config.count("server-mode")) {
        runServerMode();
    } else if (config.count("client-mode")) {
        runClientMode();
    }

    return static_cast<int>(ExitCode::OK);
}
} // namespace Sphinx
