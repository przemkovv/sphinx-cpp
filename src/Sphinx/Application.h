#pragma once

#include <string>
#include <Poco/Util/Application.h>

namespace Sphinx {
    class Application : public Poco::Util::Application
    {
        protected:
            bool helpRequested;
            void initalize(Poco::Util::Application& self) ;
            void handleHelp(const std::string& name, const std::string& value) ;
            void displayHelp() ;
            void defineOptions(Poco::Util::OptionSet& options) ;
            void configureLogger() ;
            int main(const std::vector<std::string>& args) ;


    };

}
