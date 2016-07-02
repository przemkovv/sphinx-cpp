
#pragma once
#include "HTTPClient.h"

#include "DockerContainer.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#pragma clang diagnostic ignored "-Wdocumentation"
#pragma clang diagnostic ignored "-Wswitch-enum"
#pragma clang diagnostic ignored "-Wcovered-switch-default"

#include "json.hpp"

#pragma clang diagnostic pop

#include <memory>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>

namespace Sphinx {
namespace Docker {
namespace v2 {

enum class DockerStatus {
  NoError = 0,
  BadParameter,
  ServerError,
  NoSuchContainer,
  ImpossibleToAttach,
  ContainerAlreadyStarted,
  ContainerAlreadyStopped,
  UndefiniedError
};

template <typename... T> using Result = std::tuple<DockerStatus, T...>;
using ResultJSON = Result<nlohmann::json>;

template <typename T = UnixSocket> class DockerClient {
private:
  using Socket = T;
  std::shared_ptr<HTTPClient<T>> client;

  Logger logger = make_logger("DockerClient");
  const std::string image_name = "sphinx-docker";

public:
  template <typename U = Socket,
            typename = std::enable_if_t<std::is_same<U, TCPSocket>::value>>
  explicit DockerClient(const std::string &address, unsigned short port)
    : client(std::make_shared<HTTPClient<U>>(address, port))
  {
  }

  template <typename U = Socket,
            typename = std::enable_if_t<std::is_same<U, UnixSocket>::value>>
  explicit DockerClient(const std::string &socket_path)
    : client(std::make_shared<HTTPClient<U>>(socket_path))
  {
  }

  ResultJSON list_images();
  ResultJSON list_containers();
  ResultJSON get_info();

  ResultJSON create_container(
      const std::string &image_name,
      const std::vector<std::string> &commands,
      const boost::filesystem::path &working_dir,
      const std::vector<std::pair<boost::filesystem::path, boost::filesystem::path>> &mounting_points);
  ResultJSON start_container(const Container &container);
  Result<std::string, std::string> attach_container(const Container &container);
  ResultJSON inspect_container(const Container &container);
  ResultJSON remove_container(const Container &container);
  ResultJSON stop_container(const Container &container, unsigned int wait_time);
  void run();

private:
  bool is_error(const ResultJSON &result);
  template <typename U> std::string get_message_error(const U &);
  std::string get_message_error(const nlohmann::json &data);
};

inline auto make_docker_client(const std::string &address, unsigned short port)
{
  return DockerClient<TCPSocket>{address, port};
}
inline auto make_docker_client(const std::string &socket_path)
{
  return DockerClient<UnixSocket>{socket_path};
}

} // namespace v2
} // namespace Docker
} // namespace Sphinx
