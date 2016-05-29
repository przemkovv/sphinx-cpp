
#include "ClangCompiler.h"

#include <Poco/Process.h>
#include <Poco/StreamCopier.h>
#include <Poco/PipeStream.h>

#include <range/v3/view/remove_if.hpp>
#include <range/v3/view/transform.hpp>


namespace Sphinx {
namespace Compilers {

ClangCompiler::ClangCompiler(std::string executable_path)
    : Compiler(executable_path), logger(Poco::Logger::get(name()))
{
}

std::string ClangCompiler::getVersion()
{
    auto output = run({"--version"});
    return std::get<0>(output);
}


bool ClangCompiler::compile(File file)
{
    logger.information("Compiling file: " + file.name);
    return true;
}
bool ClangCompiler::compile(Sandbox sandbox)
{
    logger.information("Compiling files: %z", sandbox.getFiles().size());

    using namespace ranges;
    auto source_files = sandbox.getFiles() 
        | view::remove_if([](const auto& file){return file.file_type != FileType::Source;})
        | view::transform([](const auto& file){ return file.full_path.toString(); });
    

    for (const auto& file_path : source_files) {
        logger.information(file_path);
    }
    
    auto compiler_args = CXXFlags;
    compiler_args.insert(compiler_args.end(), source_files.begin(), source_files.end());

    logger.information("Compiler flags: ");
    for (auto &arg : compiler_args) {
        logger.information(arg);
    }

    auto output = run(compiler_args, sandbox.getProjectRootPath().toString());
    logger.information("Compiler output: %s", std::get<0>(output));
    logger.information("Compiler error: %s", std::get<1>(output));

    return true;
}
} // namespace Compilers
} // namespace Sphinx
