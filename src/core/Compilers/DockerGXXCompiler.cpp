

#include "DockerGXXCompiler.h"

#include "utils.h"

#include <fmt/format.h>

#include <Poco/PipeStream.h>
#include <Poco/Process.h>
#include <Poco/StreamCopier.h>

#include <range/v3/view/remove_if.hpp>
#include <range/v3/view/transform.hpp>

#include <boost/filesystem.hpp>

namespace Sphinx {
namespace Compilers {

namespace fs = boost::filesystem;

DockerGXXCompiler::DockerGXXCompiler(std::string executable_path,
                                     const std::vector<std::string> &flags)
  : Compiler(executable_path),
    CXXFlags(flags),
    docker_client_(Sphinx::Docker::make_docker_client("/var/run/docker.sock")),
    logger_(make_logger(name()))
{
}

std::string DockerGXXCompiler::get_version() { return run({"--version"}).out; }

CompilerOutput DockerGXXCompiler::run(std::vector<std::string> args,
                                      std::string root_path)
{
  // TODO(przemkovv): not very efficient
  args.insert(args.begin(), executable_path_);
  logger()->debug("Launching '{}' in '{}'", args, root_path);
  auto result = docker_client_->run_command_in_mounted_dir(
      args, fs::canonical(root_path));
  return CompilerOutput{std::get<2>(result), std::get<0>(result),
                        std::get<1>(result)};
}

bool DockerGXXCompiler::compile(Sandbox sandbox)
{
  using namespace ranges;
  auto source_files =
      sandbox.files() | view::remove_if([](const auto &file) {
        return file.file_type != FileType::Source;
      }) |
      view::transform([&sandbox](const auto &file) {
        return fs::relative(file.full_path, sandbox.project_root_path())
            .string();
      });

  for (auto file : source_files) {
    logger()->info(file);
  }
  auto compiler_args = CXXFlags;
  compiler_args.insert(compiler_args.end(), source_files.begin(),
                       source_files.end());
  compiler_args.emplace_back(
      fmt::format("-o{}", fs::relative(sandbox.project_executable_path(),
                                       sandbox.project_root_path())
                              .string()));

  result_ = run(compiler_args, sandbox.project_root_path().string());
  return result_.exit_code == static_cast<int>(Sphinx::ExitCode::OK);
}
} // namespace Compilers
} // namespace Sphinx
