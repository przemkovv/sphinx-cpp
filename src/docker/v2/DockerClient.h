
#pragma once
#include "HTTPClient.h"

#include "DockerContainer.h"

#include <memory>
#include <string>
#include <vector>

namespace Sphinx {
namespace Docker {
namespace v2 {

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

  std::string list_images();
  std::string list_containers();
  std::string get_info();

  auto create_container(const std::string &image_name,
                       const std::vector<std::string> &commands,
                       const std::vector<std::string> &binds);
  bool start_container(const Container &container);
  void attach_container(const Container &container);
  void inspect_container(const Container &container);
  void remove_container(const Container &container);
  void run();
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
