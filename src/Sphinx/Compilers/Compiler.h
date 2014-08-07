
#pragma once

#include <string>
#include "Sphinx/File.h"
#include "Sphinx/Sandbox.h"

namespace Sphinx {
namespace Compilers {

class Compiler {
    protected:
        std::string executable_path;
        Compiler(std::string executable_path) :
            executable_path(executable_path) { }

        std::string output_filepath;
    public:
        std::string getVersion();
        void compile();
        virtual bool compile(File file) = 0;
        virtual bool compile(Sandbox sandbox) = 0;
        std::string getOutputFilePath() const { return output_filepath; }

        virtual std::string getOutput() const = 0;
        virtual std::string getErrors() const = 0;
};


}
}
