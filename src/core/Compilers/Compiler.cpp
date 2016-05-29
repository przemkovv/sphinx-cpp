
#include "Compiler.h"
#include <Poco/Pipe.h>
#include <Poco/Process.h>

#include "utils.h"

namespace Sphinx {
namespace Compilers {

std::tuple<std::string, std::string> Compiler::run(std::vector<std::string> args, std::string root_path) {
    Poco::Pipe out_pipe;
    Poco::Pipe err_pipe;
    auto ph = Poco::Process::launch(executable_path, args, root_path, nullptr, &out_pipe, &err_pipe);
    return std::make_tuple(to_string(out_pipe), to_string(err_pipe));
}
} // namespace Compilers
} // namespace Sphinx
