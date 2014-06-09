
#pragma once

#include <string>
#include <Poco/Logger.h>
#include "Sphinx/File.h"
#include "Compiler.h"

namespace Sphinx {
namespace Compilers {

class GXXCompiler : public Compiler {
    public:
        GXXCompiler(std::string executable_path);
        std::string getVersion();

        bool compile(File file);

        const char * name() { return "Sphinx::Compilers::GXXCompiler"; }

    private:
        Poco::Logger& logger;
};
}
}
