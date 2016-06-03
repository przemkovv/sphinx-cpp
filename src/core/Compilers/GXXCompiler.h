
#pragma once

#include <string>
#include "Logger.h"
#include "File.h"
#include "Compiler.h"

namespace Sphinx {
namespace Compilers {

class GXXCompiler : public Compiler {
    public:
        GXXCompiler(std::string executable_path);
        std::string getVersion();

        bool compile(File file);
        bool compile(Sandbox sandbox) ;

        const char *name() { return "Sphinx::Compilers::GXXCompiler"; }


    private:
        Logger logger;
};
}
}
