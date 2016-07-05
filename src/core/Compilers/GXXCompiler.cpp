
#include "GXXCompiler.h"

#include <Poco/PipeStream.h>
#include <Poco/Process.h>
#include <Poco/StreamCopier.h>

namespace Sphinx {
namespace Compilers {

GXXCompiler::GXXCompiler(std::string executable_path)
  : Compiler(executable_path), logger_(make_logger(name()))
{
}

std::string GXXCompiler::get_version()
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

bool GXXCompiler::compile(File file)
{
  logger()->info("Compiling file: " + file.name);
  return true;
}
bool GXXCompiler::compile(Sandbox sandbox)
{
  logger()->info("Compiling files: {} ", sandbox.name());
  return true;
}
} // namespace Compilers
} // namespace Sphinx
