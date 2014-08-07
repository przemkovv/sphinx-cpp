#pragma once

#include <string>
#include <istream>
#include <Poco/Logger.h>
#include <Poco/Pipe.h>
#include "Sphinx/File.h"
#include "Compiler.h"
#include "Sphinx/Sandbox.h"

namespace Sphinx {
namespace Compilers {

class MakeCompiler : public Compiler {
    public:
        MakeCompiler(std::string executable_path = "make", std::string output_filename = "application", std::string makefile_path = "data/Makefile");
        std::string getVersion();

        bool compile(File file);
        bool compile(Sandbox sandbox) ;

        const char * name() { return "Sphinx::Compilers::MakeCompiler"; }

        virtual std::string getOutput() const { return output; }
        virtual std::string getErrors() const { return errors; }
    protected:


    private:
        std::string output_filename;
        std::string makefile_path;
        std::string output;
        std::string errors;

        Poco::Logger& logger;
        std::string convertPipeToString(Poco::Pipe& pipe);

        
};
}
}
