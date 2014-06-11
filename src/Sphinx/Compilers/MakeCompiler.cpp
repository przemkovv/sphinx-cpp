

#include "MakeCompiler.h"

#include <Poco/Process.h>
#include <Poco/StreamCopier.h>
#include <Poco/PipeStream.h>

namespace Sphinx {
namespace Compilers {

MakeCompiler::MakeCompiler(std::string executable_path) : Compiler(executable_path), logger(Poco::Logger::get(name()))
{
}

std::string MakeCompiler::getVersion()
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


bool MakeCompiler::compile(File file)
{
    logger.information("Compiling file: " + file.name);
    return true;
}
bool MakeCompiler::compile(Sandbox sandbox)
{
    logger.information("Compiling files: ");
    Poco::Process::Args args;
    sandbox.copyFile(Poco::Path("data/Makefile"), FileType::Other);
    Poco::Pipe out_pipe;
    auto ph = Poco::Process::launch(executable_path,
                                    args,
                                    sandbox.getProjectRootPath().toString(),
                                    0, &out_pipe, 0);
    Poco::PipeInputStream istr(out_pipe);
    std::string output;
    Poco::StreamCopier::copyToString(istr, output);
    return true;
}
}
}
