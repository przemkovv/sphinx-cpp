
#include "Native.h"
namespace Sphinx::Executors {

CommandOutput Native::run_sync(std::string stdin) { return {stdin, "", "", 0}; }

} // namespace Sphinx::Executors
