
#include "MakeCompiler.h"

#include <Poco/PipeStream.h>
#include <Poco/Process.h>
#include <Poco/StreamCopier.h>
#include <exception>

#include <boost/filesystem.hpp>

#include "File.h"
#include "utils.h"

namespace Sphinx {
namespace Compilers {

namespace fs = boost::filesystem;

MakeCompiler::MakeCompiler(std::string executable_path,
                           std::string output_filename,
                           std::string makefile_path)
  : Compiler(executable_path),
    output_filename_{output_filename},
    makefile_path_{makefile_path},
    logger_(make_logger(name()))
{
  if (!fs::exists(makefile_path)) {
    throw std::invalid_argument("The `Makefile` is not found. (" +
                                makefile_path + ")");
  }
}

std::string MakeCompiler::get_version()
{
  Poco::Process::Args args;
  args.push_back("--version");
  Poco::Pipe out_pipe;
  auto ph = Poco::Process::launch(executable_path_, args, nullptr, &out_pipe,
                                  nullptr);
  Poco::PipeInputStream istr(out_pipe);
  std::string output;
  Poco::StreamCopier::copyToString(istr, output);
  return output;
}

bool MakeCompiler::compile(Sandbox sandbox)
{
  logger()->info("Compiling files: ");
  Poco::Process::Args args;
  sandbox.copy_file(makefile_path_, FileType::Other);
  Poco::Pipe out_pipe;
  Poco::Pipe err_pipe;
  auto ph = Poco::Process::launch(executable_path_, args,
                                  sandbox.project_root_path().string(), nullptr,
                                  &out_pipe, &err_pipe);
  output_ = convert_pipe_to_string(out_pipe);
  errors_ = convert_pipe_to_string(err_pipe);
  logger()->info(errors_);
  auto exit_code = ph.wait();
  logger()->info("Make exit code: %d", exit_code);
  output_filepath_ =
      (sandbox.project_root_path() / output_filename_.string()).string();
  return exit_code == 0;
}

std::string MakeCompiler::convert_pipe_to_string(Poco::Pipe &pipe) const
{
  Poco::PipeInputStream istr(pipe);
  std::string output;
  Poco::StreamCopier::copyToString(istr, output);
  return output;
}
} // namespace Compilers
} // namespace Sphinx
