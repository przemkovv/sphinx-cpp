

#include "DockerGXXCompiler.h"

#include "utils.h"

#include <fmt/format.h>

#include <Poco/PipeStream.h>
#include <Poco/Process.h>
#include <Poco/StreamCopier.h>

#include <range/v3/view/remove_if.hpp>
#include <range/v3/view/transform.hpp>

namespace Sphinx {
namespace Compilers {

DockerGXXCompiler::DockerGXXCompiler(std::string executable_path,
    const std::vector<std::string> &flags)
  : Compiler(executable_path), CXXFlags(flags), logger_(make_logger(name()))
{
}

std::string DockerGXXCompiler::get_version()
{
  return run({"--version"}).out;
}

bool DockerGXXCompiler::compile(Sandbox sandbox)
{
  using namespace ranges;
  auto source_files =
      sandbox.files() | view::remove_if([](const auto &file) {
        return file.file_type != FileType::Source;
      }) |
      view::transform([](const auto &file) { return file.full_path.string(); });

  for (auto file : source_files) {
    logger()->info(file);
  }
  auto compiler_args = CXXFlags;
  compiler_args.insert(compiler_args.end(), source_files.begin(),
                       source_files.end());
  compiler_args.emplace_back(
      fmt::format("-o{}", sandbox.project_executable_path().string()));

  result_ = run(compiler_args, sandbox.project_root_path().string());
  return result_.exit_code == static_cast<int>(Sphinx::ExitCode::OK);

}
} // namespace Compilers
} // namespace Sphinx
