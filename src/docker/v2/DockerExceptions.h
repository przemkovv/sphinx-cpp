
#pragma once

#include "HTTPResponse.h"
#include "DockerContainer.h"

#include <fmt/format.h>
#include <stdexcept>

namespace Sphinx {
namespace Docker {
namespace v2 {


class docker_exception : public std::runtime_error {
public:
  docker_exception(const std::string &message)
    : runtime_error(message)
  {
  }
};
class cannot_create_container_exception : public std::runtime_error {
public:
  cannot_create_container_exception(const HTTPResponse &response)
    : runtime_error(fmt::format("Could not create container.\n{0}\n",
                                response.dump()))
  {
  }
};
class container_doesnt_exist_exception : public std::runtime_error {
public:
  container_doesnt_exist_exception(const Container &container)
    : runtime_error(
          fmt::format("Container with id: {0} doesn't exist", container.id))
  {
  }
};

class server_error : public std::runtime_error {
public:
  server_error(const HTTPResponse &response)
    : runtime_error(fmt::format("Server error.\n{0}", response.dump()))
  {
  }
};

class undefined_error : public std::runtime_error {
public:
  undefined_error(const HTTPResponse &response)
    : runtime_error(
          fmt::format("Undefined error. \n{0}\n", response.dump()))
  {
  }
};

} // namespace v2
} // namespace Docker
} // namespace Sphinx

