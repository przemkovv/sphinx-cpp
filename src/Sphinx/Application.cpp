
#include "Application.h"


#include <thread>
#include <chrono>
#include <iostream>
#include <Poco/Util/HelpFormatter.h>

#include <Poco/ConsoleChannel.h>
#include <Poco/FormattingChannel.h>
#include <Poco/PatternFormatter.h>
#include <Poco/AsyncChannel.h>

#include "Sphinx/Net/Server.h"
#include "Sphinx/Compilers/MakeCompiler.h"

#include "Sphinx/File.h"
#include "Sphinx/Sandbox.h"

using Poco::AutoPtr;
using Poco::Util::Option;
using Poco::Util::OptionCallback;
using Poco::Logger;
using Poco::ConsoleChannel;
using Poco::PatternFormatter;
using Poco::AsyncChannel;
using Poco::FormattingChannel;

namespace Sphinx {

void Application::initalize(Poco::Util::Application& self)
{
    Poco::Util::Application::initialize(self);
}

void Application::handleHelp(const std::string& name, const std::string& value)
{
    helpRequested = true;
    displayHelp();
    stopOptionsProcessing();
}

void Application::displayHelp()
{
    Poco::Util::HelpFormatter helpFormatter(options());
    helpFormatter.setCommand(commandName());
    helpFormatter.setUsage("OPTIONS");
    helpFormatter.format(std::cout);
}

void Application::defineOptions(Poco::Util::OptionSet& options)
{
    Poco::Util::Application::defineOptions(options);
    options.addOption(Option("help", "h", "display help information")
                      .required(false)
                      .repeatable(false)
                      .callback(OptionCallback<Application> (this, &Application::handleHelp)));
    options.addOption(Option("client", "c", "client mode")
                      .required(false)
                      .repeatable(false)
                      .binding("application.client-mode"));
    options.addOption(Option("server", "s", "server mode")
                      .required(false)
                      .repeatable(false)
                      .binding("application.server-mode"));
}

void Application::configureLogger()
{
    AutoPtr<ConsoleChannel> console(new ConsoleChannel);
    AutoPtr<PatternFormatter> pattern_formatter(new PatternFormatter);
    pattern_formatter->setProperty("pattern", "%Y-%m-%d %H:%M:%S %s: %t");
    AutoPtr<FormattingChannel> formatting_channel(new FormattingChannel(pattern_formatter, console));
    AutoPtr<AsyncChannel> asyncChannel(new AsyncChannel(formatting_channel));
    Logger::root().setChannel(asyncChannel);
    logger().setChannel(asyncChannel);
}

void Application::runServerMode()
{
    logger().information("I'm a server");
    Net::Server server;
    server.listen();

    while (!terminate.tryWait(1000)) {
        logger().information("Going sleep...");
    }
}

void Application::runClientMode()
{
    logger().information("I'm a client");
    Compilers::MakeCompiler compiler;
    logger().information(compiler.getVersion());
    File file {"main.cpp",  R"code(
#include <iostream>
int main() {
    std::cout << "Hello World" << std::endl;
    return 0;
} )code"
              };
    // 1. prepare sandbox
    logger().information("Environment preparation");
    Sandbox sandbox;
    sandbox.addFile(file);
    // 2. compile
    compiler.compile(sandbox);
    logger().information("Compilation was completed");
}

int Application::main(const std::vector<std::string>& args)
{
    if (!helpRequested) {
        configureLogger();

        if (config().has("application.server-mode")) {
            runServerMode();
        } else if (config().has("application.client-mode")) {
            runClientMode();
        }
    }

    return EXIT_OK;
}
}
