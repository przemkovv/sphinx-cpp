
#pragma once

#include <memory>
#include <string>
#include <spdlog/spdlog.h>

namespace Sphinx {

typedef std::shared_ptr<spdlog::logger> Logger;

inline auto make_logger(std::string name) {
    return spdlog::stdout_logger_st(name, true);
}

} //namespace Sphinx
