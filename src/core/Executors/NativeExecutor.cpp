
#include "NativeExecutor.h"

#include "process.h"
namespace Sphinx::Executors {

ExitCode NativeExecutor::run(const std::vector<std::string> & args)
{

  command_.exec(args);
  command_.wait();

  auto exit_code = command_.code();


  return exit_code;
}
IOStreams &NativeExecutor::get_io_streams()
{
  return io_streams_;
}
void NativeExecutor::set_timeout(int /*time*/) {}

} // namespace Sphinx::Executors
