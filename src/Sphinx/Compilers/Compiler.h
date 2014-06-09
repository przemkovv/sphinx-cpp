
#pragma once

#include <string>
#include "Sphinx/File.h"

namespace Sphinx {
namespace Compilers {

class Compiler {
    protected:
        std::string executable_path;
        Compiler(std::string executable_path) :
            executable_path(executable_path) { }
    public:
        std::string getVersion();
        void compile();
        virtual bool compile(File file) = 0;
};


}
}
