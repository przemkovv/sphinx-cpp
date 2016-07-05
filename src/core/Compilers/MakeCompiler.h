#pragma once

#include <string>
#include <istream>
#include <Poco/Pipe.h>
#include "File.h"
#include "Compiler.h"
#include "Sandbox.h"
#include "Logger.h"

namespace Sphinx {
namespace Compilers {

class MakeCompiler : public Compiler {
    public:
        MakeCompiler(std::string executable_path = "make",
                     std::string output_filename = "application",
                     std::string makefile_path = "data/Makefile");
        std::string get_version() override;

        bool compile(File file) override;
        bool compile(Sandbox sandbox)  override;

        const char *name() const override { return "Sphinx::Compilers::MakeCompiler"; }

        virtual const std::string& get_output() const override { return output_; }
        virtual const std::string& get_errors() const  override{ return errors_; }
    protected:


    private:
        boost::filesystem::path output_filename_;
        boost::filesystem::path makefile_path_;
        std::string output_;
        std::string errors_;

        Logger logger_;
        std::string convert_pipe_to_string(Poco::Pipe& pipe) const;

    protected:
        virtual Logger& logger() override { return logger_; }



};
}
}
