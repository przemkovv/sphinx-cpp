
#include "MakeCompiler.h"

#include <exception>
#include <Poco/Process.h>
#include <Poco/StreamCopier.h>
#include <Poco/PipeStream.h>

#include <boost/filesystem.hpp>

#include "utils.h"
#include "File.h"


namespace Sphinx {
namespace Compilers {

namespace fs = boost::filesystem;

MakeCompiler::MakeCompiler(std::string executable_path, std::string output_filename, std::string makefile_path)
    : Compiler(executable_path),
      output_filename{output_filename},
      makefile_path{makefile_path},
      logger(make_logger(name()))
{
    if (!fs::exists(makefile_path)) {
        throw std::invalid_argument("The `Makefile` is not found. ("+makefile_path+")");
    }
}

std::string MakeCompiler::getVersion()
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


bool MakeCompiler::compile(File file)
{
    logger->info("Compiling file: " + file.name);
    return true;
}
bool MakeCompiler::compile(Sandbox sandbox)
{
    logger->info("Compiling files: ");
    Poco::Process::Args args;
    sandbox.copyFile(makefile_path, FileType::Other);
    Poco::Pipe out_pipe;
    Poco::Pipe err_pipe;
    auto ph = Poco::Process::launch(executable_path,
                                    args,
                                    sandbox.getProjectRootPath().string(),
                                    nullptr, &out_pipe, &err_pipe);
    output = convertPipeToString(out_pipe);
    errors = convertPipeToString(err_pipe);
    logger->info(errors);
    auto exit_code = ph.wait();
    logger->info("Make exit code: %d", exit_code);
    output_filepath = (sandbox.getProjectRootPath() / output_filename.string()).string();
    return exit_code == 0;
}

std::string MakeCompiler::convertPipeToString(Poco::Pipe& pipe)
{
    Poco::PipeInputStream istr(pipe);
    std::string output;
    Poco::StreamCopier::copyToString(istr, output);
    return output;
}
} // namespace Compilers
} // namespace Sphinx
