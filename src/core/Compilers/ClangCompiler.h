
#pragma once

#include <string>
#include <Poco/Logger.h>
#include "File.h"
#include "Compiler.h"

namespace Sphinx {
namespace Compilers {

class ClangCompiler : public Compiler {
    public:
        ClangCompiler(std::string executable_path);
        std::string getVersion();

        bool compile(File file);
        bool compile(Sandbox sandbox) ;

        const char * name() { return "Sphinx::Compilers::ClangCompiler"; }

        std::string getOutput() const { return ""; }
        std::string getErrors() const { return ""; }


    private:
        Poco::Logger& logger;
};
}
}
