
#pragma once


#include <string>
#include "Sphinx/File.h"
#include "Compiler.h"

namespace Sphinx {
namespace Compilers {

class ClangParser : public Compiler {
    public:
        ClangParser();
        std::string getVersion();

        bool compile(File file);
        bool compile(Sandbox sandbox) ;

        const char * name() { return "Sphinx::Compilers::ClangParser"; }

        virtual std::string getOutput() const ;
        virtual std::string getErrors() const ;


    private:
        std::shared_ptr<spdlog::logger> logger;
};
}
}
