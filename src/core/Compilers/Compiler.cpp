
#include "Compiler.h"
#include "utils.h"

#include "process.h"
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
  logger()->debug("Launching '{}' with arguments {} in '{}'", executable_path_, args, root_path);

  procxx::process compiler(executable_path_, root_path);
  compiler.append_arguments(args.begin(), args.end());
  compiler.exec();
  compiler.wait();

  auto exit_code = compiler.code();

  std::string stdout(std::istreambuf_iterator<char>(compiler.output()), {});
  std::string stderr(std::istreambuf_iterator<char>(compiler.error()), {});

  return CompilerOutput{exit_code, stdout, stderr};
}
} // namespace Compilers
} // namespace Sphinx
