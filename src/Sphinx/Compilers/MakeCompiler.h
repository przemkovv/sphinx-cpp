#pragma once

#include <string>
#include <Poco/Logger.h>
#include "Sphinx/File.h"
#include "Compiler.h"
#include "Sphinx/Sandbox.h"

namespace Sphinx {
namespace Compilers {

class MakeCompiler : public Compiler {
    public:
        MakeCompiler(std::string executable_path = "make");
        std::string getVersion();

        bool compile(File file);
        bool compile(Sandbox sandbox) ;

        const char * name() { return "Sphinx::Compilers::MakeCompiler"; }


    private:
        Poco::Logger& logger;
};
}
}
