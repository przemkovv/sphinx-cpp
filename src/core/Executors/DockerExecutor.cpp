
#include "DockerExecutor.h"

#include "process.h"

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

namespace Sphinx::Executors {

CommandOutput DockerExecutor::run_sync(std::string stdin)
{
  std::vector<std::string> command_args{
      fs::relative(sandbox_.project_executable_path(),
                   sandbox_.project_root_path())
          .string()};

  auto result = docker_client_->run_command_in_mounted_dir(
      command_args, fs::canonical(sandbox_.project_root_path()));

  return {stdin, std::get<0>(result), std::get<1>(result), std::get<2>(result)};
}

} // namespace Sphinx::Executors
