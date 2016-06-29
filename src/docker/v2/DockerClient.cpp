
#include "DockerClient.h"

#include "DockerExceptions.h"

#include "HTTPCommon.h"
#include <fmt/format.h>

#include <chrono>
#include <future>
#include <thread>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#pragma clang diagnostic ignored "-Wdocumentation"
#pragma clang diagnostic ignored "-Wswitch-enum"
#pragma clang diagnostic ignored "-Wcovered-switch-default"

#include "json.hpp"

#pragma clang diagnostic pop

namespace Sphinx {
namespace Docker {
namespace v2 {

using json = nlohmann::json;

template <typename T> std::string DockerClient<T>::getImages()
{
  auto data = client->get("/images/json?all=1").data();
  auto images = json::parse(data);
  return images.dump(4);
}
template <typename T> std::string DockerClient<T>::getContainers()
{
  auto data = client->get("/containers/json?all=1&size=1").data();
  auto containers = json::parse(data);
  return containers.dump(4);
}
template <typename T> std::string DockerClient<T>::getInfo()
{
  auto data = client->get("/info").data();
  auto j1 = json::parse(data);
  return j1.dump(4);
}
template <typename T>
auto DockerClient<T>::createContainer(const std::string &image_name,
                                      const std::vector<std::string> &commands)
{
  json container = {{"Hostname", ""},
                    {"User", ""},
                    {"Memory", 0},
                    {"MemorySwap", 0},
                    {"AttachStdin", true},
                    {"AttachStdout", true},
                    {"AttachStderr", true},
                    {"Tty", false},
                    {"OpenStdin", true},
                    {"StdinOnce", true},
                    {"Cmd", commands},
                    {"Image", image_name},
                    {
                        "WorkingDir", "",
                    }};
  const auto request_data = container.dump();
  auto response = client->post("/containers/create", request_data);
  logger->info("Create container: {0}", response.dump());

  if (response.status() == HTTPStatus::CREATED) {
    auto data_json = json::parse(response.data());
    Container container{data_json["Id"]};
    return container;
  }
  throw cannot_create_container_exception{response};
}

template <typename T>
bool DockerClient<T>::startContainer(const Container &container)
{
  auto request = fmt::format("/containers/{0}/start", container.id);
  auto response = client->post(request);
  logger->info("Start container: {0}", response.dump());
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wswitch-enum"
  switch (response.status()) {
  case HTTPStatus::NOT_FOUND: {
    throw container_doesnt_exist_exception{container};
  }

  case HTTPStatus::SERVER_ERROR: {
    throw server_error{response};
  }

  case HTTPStatus::NOT_MODIFIED:
  case HTTPStatus::NO_CONTENT: {
    return true;
  }

  default:
    throw undefined_error{response};
  }
#pragma clang diagnostic pop
}

template <typename Task1, typename Task2>
auto while_do(const Task1 &task_while, const Task2 &task_do) {

  auto future = std::async(std::launch::async, task_while);
  std::future_status status;
  do {
    task_do();
    status = future.wait_for(std::chrono::milliseconds(5));
  } while (status != std::future_status::ready);

  return future.get();
}

template <typename T>
void DockerClient<T>::attachContainer(const Container &container)
{
  auto request = fmt::format(
      "/containers/{0}/attach?stream=1&logs=1&stdout=1&stderr=1", container.id);
  boost::asio::streambuf output;
  boost::asio::streambuf error;
  client->set_output_stream(&output);
  client->set_error_stream(&error);
  client->use_output_streams(true);

  std::istream output_stream(&output);

  auto response = while_do([&]() {
    return client->post(request, "", {{"Upgrade", "tcp"}});
  },
  [&]() {
    std::string data{std::istreambuf_iterator<char>(output_stream), {}};
    if (!data.empty()) {
      logger->info("Attach container: {0}", data);
    }
    });

  client->use_output_streams(false);
}

template <typename T>
void DockerClient<T>::inspectContainer(const Container &container)
{
  auto query_path = fmt::format("/containers/{0}/json", container.id);
  auto response = client->get(query_path);

  if (response.status() == HTTPStatus::OK) {
    auto data = json::parse(response.data());
    logger->info("Inspect container: {0}", data.dump(2));
  }
  else {
    logger->info(
        "Inspecting container: {0} failed (status code: {1} {2}).\nData: {3}",
        container.id, static_cast<uint32_t>(response.status()),
        response.status_message(), response.data());
  }
}
template <typename T>
void DockerClient<T>::deleteContainer(const Container &container)
{
  auto query_path = fmt::format("/containers/{0}", container.id);
  auto response = client->request(HTTPMethod::DELETE, query_path);
  if (response.status() == HTTPStatus::NO_CONTENT) {
    logger->info("Removed container: {0}", container.id);
  }
  else {
    logger->info("Removing container: {0} failed (status code: {1} {2}).",
                 container.id, static_cast<uint32_t>(response.status()),
                 response.status_message());
  }
}

template <typename T> void DockerClient<T>::run()
{
  auto container = createContainer(
      image_name,
      {"/bin/zsh", "-c",
       "count=1; repeat 20 { echo $count && sleep 1; (( count++ )) } "});

  // auto container = createContainer(image_name, {"date"});
  // inspectContainer(container);
  startContainer(container);
  attachContainer(container);
  inspectContainer(container);
  deleteContainer(container);
  inspectContainer(container);
  // getContainers();
  // 1. Create the container
  // 2. If the status code is 404, it means the image doesn’t exist:
  //    a. Try to pull it.
  //    b. Then, retry to create the container.
  // 3. Start the container.
  // 4. If you are not in detached mode:
  // 5. Attach to the container, using logs=1 (to have stdout and stderr from
  // the container’s start) and stream=1
  // 6. If in detached mode or only stdin is attached, display the container’s
  // id.
}

template class DockerClient<UnixSocket>;
template class DockerClient<TCPSocket>;

} // namespace v2
} // namespace Docker
} // namespace Sphinx
