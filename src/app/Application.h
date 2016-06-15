#pragma once

#include "Logger.h"
#include <boost/program_options.hpp>
#include <string>


namespace Sphinx {

class Application {
    protected:
        void handleHelp(const std::string& name, const std::string& value) ;
        void displayHelp() ;
        void configureLogger(spdlog::level::level_enum level) ;
        void runClientMode();
        [[noreturn]] void runServerMode();

        boost::program_options::options_description prepare_options_description();
        boost::program_options::variables_map parse_command_line_options(const std::vector<std::string>& arguments);
    private:
        Logger logger;

        const std::vector<std::string> args;
        boost::program_options::options_description options_description;
        boost::program_options::variables_map config;
    public:
        Application(const std::vector<std::string>& args);
        int run();

};

}
