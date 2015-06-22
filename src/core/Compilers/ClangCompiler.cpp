
#include "ClangCompiler.h"

#include <Poco/Process.h>
#include <Poco/StreamCopier.h>
#include <Poco/PipeStream.h>

namespace Sphinx {
namespace Compilers {

ClangCompiler::ClangCompiler(std::string executable_path) : Compiler(executable_path), logger(Poco::Logger::get(name()))
{
}

std::string ClangCompiler::getVersion()
{
    Poco::Process::Args args;
    args.push_back("--version");
    Poco::Pipe out_pipe;
    auto ph = Poco::Process::launch(executable_path, args, 0, &out_pipe, 0);
    Poco::PipeInputStream istr(out_pipe);
    std::string output;
    Poco::StreamCopier::copyToString(istr, output);
    return output;
}


bool ClangCompiler::compile(File file)
{
    logger.information("Compiling file: " + file.name);
    return true;
}
bool ClangCompiler::compile(Sandbox sandbox)
{
    logger.information("Compiling files: ");
    return true;
}
}
}
