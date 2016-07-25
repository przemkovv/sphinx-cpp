
#include "Native.h"

#include "process.h"
namespace Sphinx::Executors {

CommandOutput Native::run_sync(std::string stdin)
{

  procxx::process command(sandbox_.project_executable_path().string(),
                          sandbox_.project_root_path().string());

  command.input() << stdin;
  command.exec();
  command.wait();

  auto exit_code = command.code();

  std::string stdout(std::istreambuf_iterator<char>(command.output()), {});
  std::string stderr(std::istreambuf_iterator<char>(command.error()), {});

  return {stdin, stdout, stderr, exit_code};
}

} // namespace Sphinx::Executors
