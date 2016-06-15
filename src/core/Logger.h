
#pragma once

#include <spdlog/spdlog.h>

#include <memory>
#include <string>

namespace Sphinx {

typedef std::shared_ptr<spdlog::logger> Logger;

inline auto make_logger(std::string name)
{
    try {
        return spdlog::stdout_logger_st(name, true);
    } catch (spdlog::spdlog_ex& ) {
        return spdlog::get(name);
    }
}

} //namespace Sphinx
