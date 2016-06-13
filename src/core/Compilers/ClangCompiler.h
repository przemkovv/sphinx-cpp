
#pragma once

#include <memory>
#include <string>
#include <tuple>
#include <vector>
#include "Logger.h"
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
        std::string getVersion() override;

        bool compile(File file) override;
        bool compile(Sandbox sandbox)  override;

        const char *name() override { return "Sphinx::Compilers::ClangCompiler"; }

        std::string getOutput() const override { return result.out; }
        std::string getErrors() const override{ return result.err; }


    private:
        CompilerOutput result;
        Logger logger;
};
}
}
