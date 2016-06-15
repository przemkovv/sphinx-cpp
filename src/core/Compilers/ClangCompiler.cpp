
#include "ClangCompiler.h"

#include "utils.h"

#include <fmt/format.h>

#include <Poco/Process.h>
#include <Poco/StreamCopier.h>
#include <Poco/PipeStream.h>

#include <range/v3/view/remove_if.hpp>
#include <range/v3/view/transform.hpp>



namespace Sphinx {
namespace Compilers {

ClangCompiler::ClangCompiler(std::string executable_path, const std::vector<std::string> &flags)
    : Compiler(executable_path), CXXFlags(flags), logger(make_logger(name()))
    //: Compiler(executable_path), logger(Poco::Logger::get(name()))
{
}

std::string ClangCompiler::getVersion()
{
    auto result = run({"--version"});
    return result.out;
}

bool ClangCompiler::compile(File file)
{
    logger->info("Compiling file: " + file.name);
    return true;
}

bool ClangCompiler::compile(Sandbox sandbox)
{
    using namespace ranges;
    auto source_files = sandbox.getFiles() 
        | view::remove_if([](const auto& file){return file.file_type != FileType::Source;})
        | view::transform([](const auto& file){ return file.full_path.string(); });

    for (auto file : source_files) {
        logger->info(file);
    }
    auto compiler_args = CXXFlags;
    compiler_args.insert(compiler_args.end(), source_files.begin(), source_files.end());

    result = run(compiler_args, sandbox.getProjectRootPath().string());
    return result.exit_code == static_cast<int>(Sphinx::ExitCode::OK);
}

} // namespace Compilers
} // namespace Sphinx
