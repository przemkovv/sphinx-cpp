
#include "Application.h"


#include <thread>
#include <chrono>
#include <iostream>
#include <Poco/Util/HelpFormatter.h>

#include <Poco/ConsoleChannel.h>
#include <Poco/FormattingChannel.h>
#include <Poco/PatternFormatter.h>
#include <Poco/AsyncChannel.h>

#include "Server.h"
#include "compiler.h"

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
    logger().information("I'm server");
    Server server;
    server.listen();

    while (!terminate.tryWait(1000)) {
        logger().information("Going sleep...");
    }
}

void Application::runClientMode()
{
    logger().information("I'm client");
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

        compiler::GXXCompiler compiler("/usr/bin/g++");
        logger().information(compiler.getVersion());
    }

    return EXIT_OK;
}
}