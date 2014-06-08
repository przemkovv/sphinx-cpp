
#include "compiler.h"

namespace compiler {

    GXXCompiler::GXXCompiler(std::string executable_path) : Compiler(executable_path) {
    }

    std::string GXXCompiler::getVersion() {
                Poco::Process::Args args;
                args.push_back("--version");

                Poco::Pipe out_pipe;
                auto ph = Poco::Process::launch(executable_path, args, 0, &out_pipe, 0);
                Poco::PipeInputStream istr(out_pipe);

                std::string output;
                Poco::StreamCopier::copyToString(istr, output);
                return output;
    }

        
}
