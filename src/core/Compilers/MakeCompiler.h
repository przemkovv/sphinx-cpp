#pragma once

#include <string>
#include <istream>
#include <Poco/Pipe.h>
#include "File.h"
#include "Compiler.h"
#include "Sandbox.h"
#include "Logger.h"

namespace Sphinx {
namespace Compilers {

class MakeCompiler : public Compiler {
    public:
        MakeCompiler(std::string executable_path = "make",
                     std::string output_filename = "application",
                     std::string makefile_path = "data/Makefile");
        std::string getVersion();

        bool compile(File file);
        bool compile(Sandbox sandbox) ;

        const char *name() { return "Sphinx::Compilers::MakeCompiler"; }

        virtual std::string getOutput() const { return output; }
        virtual std::string getErrors() const { return errors; }
    protected:


    private:
        boost::filesystem::path output_filename;
        boost::filesystem::path makefile_path;
        std::string output;
        std::string errors;

        Logger logger;
        std::string convertPipeToString(Poco::Pipe& pipe);


};
}
}
