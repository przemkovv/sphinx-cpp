
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

class DockerClient {
public:
  virtual ResultJSON list_images() = 0;
  virtual ResultJSON list_containers() = 0;
  virtual ResultJSON get_info() = 0;

  virtual ResultJSON create_container(
      const std::string &image_name,
      const std::vector<std::string> &commands,
      const boost::filesystem::path &working_dir,
      const std::vector<
          std::pair<boost::filesystem::path, boost::filesystem::path>>
          &mounting_points) = 0;
  virtual ResultJSON start_container(const Container &container) = 0;
  virtual Result<std::string, std::string>
  attach_container(const Container &container) = 0;
  virtual ResultJSON inspect_container(const Container &container) = 0;
  virtual ResultJSON remove_container(const Container &container) = 0;
  virtual ResultJSON stop_container(const Container &container,
                                    unsigned int wait_time) = 0;
  virtual ResultJSON wait_container(const Container &container) = 0;

  virtual std::tuple<std::string, std::string, int>
  run_command_in_mounted_dir(const std::vector<std::string> &cmd,
                             const boost::filesystem::path &mount_dir) = 0;

  virtual ~DockerClient() {}

protected:
  DockerClient() {}
  DockerClient(const DockerClient &) = default;
  DockerClient(DockerClient &&) = default;
};

template <typename T = UnixSocket>
class DockerSocketClient : public DockerClient {
private:
  using Socket = T;
  std::shared_ptr<HTTPClient<T>> client;

  Logger logger = make_logger("Sphinx::Docker::v2::DockerSocketClient");
  const std::string image_name = "sphinx-docker";

public:
  template <typename U = Socket,
            typename = std::enable_if_t<std::is_same<U, TCPSocket>::value>>
  explicit DockerSocketClient(const std::string &address,
                              unsigned short port,
                              const std::string &docker_image_name)
    : client(std::make_shared<HTTPClient<U>>(address, port)),
      image_name(docker_image_name)
  {
  }

  template <typename U = Socket,
            typename = std::enable_if_t<std::is_same<U, UnixSocket>::value>>
  explicit DockerSocketClient(const std::string &socket_path,
                              const std::string &docker_image_name)
    : client(std::make_shared<HTTPClient<U>>(socket_path)),
      image_name(docker_image_name)
  {
  }

  ResultJSON list_images() override;
  ResultJSON list_containers() override;
  ResultJSON get_info() override;

  ResultJSON create_container(
      const std::string &image_name,
      const std::vector<std::string> &commands,
      const boost::filesystem::path &working_dir,
      const std::vector<
          std::pair<boost::filesystem::path, boost::filesystem::path>>
          &mounting_points) override;
  ResultJSON start_container(const Container &container) override;
  Result<std::string, std::string>
  attach_container(const Container &container) override;
  ResultJSON inspect_container(const Container &container) override;
  ResultJSON remove_container(const Container &container) override;
  ResultJSON stop_container(const Container &container,
                            unsigned int wait_time) override;
  ResultJSON wait_container(const Container &container) override;

  std::tuple<std::string, std::string, int>
  run_command_in_mounted_dir(const std::vector<std::string> &cmd,
                             const boost::filesystem::path &mount_dir) override;

  DockerSocketClient(const DockerSocketClient<T> &) = default;
  DockerSocketClient(DockerSocketClient<T> &&) = default;
  virtual ~DockerSocketClient() {}

private:
  template <typename... U> bool is_error(const Result<U...> &result);
  template <typename... U> bool throw_if_error(const Result<U...> &result);
  template <typename U> std::string get_message_error(const U &data);
  std::string get_message_error(const nlohmann::json &data);
};

inline auto make_docker_client(const std::string &address, unsigned short port, const std::string &image_name)
{
  return std::make_unique<DockerSocketClient<TCPSocket>>(address, port, image_name);
}
inline auto make_docker_client(const std::string &socket_path, const std::string &image_name)
{
  return std::make_unique<DockerSocketClient<UnixSocket>>(socket_path, image_name);
}

} // namespace v2
} // namespace Docker
} // namespace Sphinx
