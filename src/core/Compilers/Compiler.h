
#pragma once

#include "Logger.h"
#include "File.h"
#include "Sandbox.h"

#include <string>
#include <tuple>
#include <vector>

namespace Sphinx {
namespace Compilers {

struct CompilerOutput {
    int exit_code;
    std::string out;
    std::string err;
};

class Compiler {
    protected:
        std::string executable_path;
        Compiler(std::string executable_path);

        CompilerOutput run(std::vector<std::string> args, std::string root_path = ".");

        std::string output_filepath;

        virtual Logger& getLogger() = 0;

    public:
        std::string getOutputFilePath() const { return output_filepath; }
        virtual std::string getVersion() = 0;
        void compile();
        virtual bool compile(File file) = 0;
        virtual bool compile(Sandbox sandbox) = 0;

        virtual std::string getOutput() const = 0;
        virtual std::string getErrors() const = 0;

        virtual const char *name() { return "Sphinx::Compilers::Compiler"; }

        virtual ~Compiler() { }

        
};


}
}
