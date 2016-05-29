
#pragma once

#include <string>
#include <tuple>
#include <vector>
#include <Poco/Logger.h>
#include "File.h"
#include "Compiler.h"

namespace Sphinx {
namespace Compilers {

class ClangCompiler : public Compiler {
    public:
        const std::vector<std::string> CXXFlags {
            "-std=c++14", "-Wall", "-Werror"
        };
            
        ClangCompiler(std::string executable_path);
        std::string getVersion();

        bool compile(File file);
        bool compile(Sandbox sandbox) ;

        const char *name() { return "Sphinx::Compilers::ClangCompiler"; }

        std::string getOutput() const { return ""; }
        std::string getErrors() const { return ""; }


    private:
        Poco::Logger& logger;
};
}
}
