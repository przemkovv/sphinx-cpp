
#include "GXXCompiler.h"

#include <Poco/Process.h>
#include <Poco/StreamCopier.h>
#include <Poco/PipeStream.h>

namespace Sphinx {
namespace Compilers {

GXXCompiler::GXXCompiler(std::string executable_path)
    : Compiler(executable_path),
      logger(Poco::Logger::get(name()))
{
}

std::string GXXCompiler::getVersion()
{
    Poco::Process::Args args;
    args.push_back("--version");
    Poco::Pipe out_pipe;
    auto ph = Poco::Process::launch(executable_path, args, nullptr, &out_pipe, nullptr);
    Poco::PipeInputStream istr(out_pipe);
    std::string output;
    Poco::StreamCopier::copyToString(istr, output);
    return output;
}


bool GXXCompiler::compile(File file)
{
    logger.information("Compiling file: " + file.name);
    return true;
}
bool GXXCompiler::compile(Sandbox sandbox)
{
    logger.information("Compiling files: ");
    return true;
}
} // namespace Compilers
} // namespace Sphinx
