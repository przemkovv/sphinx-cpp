
#include "DockerExecutor.h"

#include "process.h"

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

namespace Sphinx::Executors {

ExitCode DockerExecutor::run(const std::vector<std::string> & args)
{
  std::vector<std::string> command_args{
      fs::relative(sandbox_.project_executable_path(),
                   sandbox_.project_root_path())
          .string()};
  command_args.insert(command_args.end(), args.begin(), args.end());

  auto result = docker_client_->run_command_in_mounted_dir(
      command_args, fs::canonical(sandbox_.project_root_path()), io_buffers_);

  return result;
}
IOStreams &DockerExecutor::get_io_streams() { return io_streams_; }

void DockerExecutor::set_timeout(int /*time*/) {}

} // namespace Sphinx::Executors
