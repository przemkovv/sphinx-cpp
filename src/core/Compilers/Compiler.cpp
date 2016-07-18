
#include "Compiler.h"
#include "utils.h"

#include <Poco/Pipe.h>
#include <Poco/Process.h>
#include <fmt/format.h>

namespace Sphinx {
namespace Compilers {

namespace fs = ::boost::filesystem;

Compiler::Compiler(std::string executable_path)
  : executable_path_(executable_path)
{
  if (!fs::exists(executable_path_)) {
    throw std::invalid_argument(
        fmt::format("The binary '{}' doesn't exist (compiler {})",
                    executable_path_, name()));
  }
}

CompilerOutput Compiler::run(std::vector<std::string> args,
                             std::string root_path)
{
  Poco::Pipe out_pipe;
  Poco::Pipe err_pipe;
  logger()->debug("Launching '{}' with arguments {} in '{}'", executable_path_, args, root_path);
  auto ph = Poco::Process::launch(executable_path_, args, root_path, nullptr,
                                  &out_pipe, &err_pipe);
  auto exit_code = ph.wait();
  return CompilerOutput{exit_code, to_string(out_pipe), to_string(err_pipe)};
}
} // namespace Compilers
} // namespace Sphinx
