
#include "HTTPClient.h"

#include "Logger.h"

#include <fmt/format.h>
#include <boost/asio.hpp>

int main()
{
    spdlog::set_level(spdlog::level::debug);
    auto logger = Sphinx::make_logger("docker");
    logger->info("Hello");
    Sphinx::Docker::HTTPClient connection{"127.0.0.1", 2375};
    auto response = connection.get("/info");
    logger->info("Response received:\n{} ", response);
    return 0;
}
