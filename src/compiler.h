
#pragma once

#include <string>
#include <Poco/Process.h>
#include <Poco/StreamCopier.h>
#include <Poco/PipeStream.h>



namespace compiler {
class Compiler {
    protected:
        std::string executable_path;
        Compiler(std::string executable_path) :
            executable_path(executable_path) { }
    public:
        std::string getVersion();
        void compile();
};

class GXXCompiler : public Compiler {
    public:
        GXXCompiler(std::string executable_path);
        std::string getVersion();
};

class ClangCompiler {

};
}
