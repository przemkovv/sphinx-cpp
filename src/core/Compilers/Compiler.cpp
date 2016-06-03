
#include "Compiler.h"
#include <fmt/format.h>
#include <Poco/Pipe.h>
#include <Poco/Process.h>

#include "utils.h"

namespace Sphinx {
namespace Compilers {

Compiler::Compiler(std::string executable_path) :
     executable_path(executable_path)
{
    if (!Poco::File(executable_path).exists()) {
        throw std::invalid_argument(fmt::format("The binary '{}' doesn't exist (compiler {})",
                                                executable_path, name()));
    }
}

CompilerOutput Compiler::run(std::vector<std::string> args, std::string root_path)
{
    Poco::Pipe out_pipe;
    Poco::Pipe err_pipe;
    auto ph = Poco::Process::launch(executable_path, args, root_path, nullptr, &out_pipe, &err_pipe);
    auto exit_code = ph.wait();
    return CompilerOutput{exit_code, to_string(out_pipe), to_string(err_pipe)};
}
} // namespace Compilers
} // namespace Sphinx
