
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

  std::string getImages();
  std::string getContainers();
  std::string getInfo();

  auto createContainer(const std::string &image_name,
                       const std::vector<std::string> &commands);
  bool startContainer(const Container &container);
  void attachContainer(const Container &container);
  void inspectContainer(const Container &container);
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
