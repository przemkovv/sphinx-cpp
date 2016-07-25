
#include "MakeCompiler.h"

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
  return run({"--version"}).out;
}

bool MakeCompiler::compile(Sandbox /*sandbox*/)
{
  logger()->error("Unimplemented feature.");
  return 0;
}

} // namespace Compilers
} // namespace Sphinx
