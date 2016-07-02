
#include "DockerClient.h"

#include "DockerExceptions.h"

#include "HTTPCommon.h"
#include <fmt/format.h>

#include <boost/filesystem.hpp>
#include <chrono>
#include <future>
#include <thread>

namespace Sphinx {
namespace Docker {
namespace v2 {

using json = nlohmann::json;
namespace fs = boost::filesystem;

template <typename T> ResultJSON DockerClient<T>::list_images()
{
  auto response = client->get("/images/json?all=1");
  auto images = json::parse(response.data());
  return {response.status(), images};
}
template <typename T> ResultJSON DockerClient<T>::list_containers()
{
  auto response = client->get("/containers/json?all=1&size=1");
  auto containers = json::parse(response.data());
  return {response.status(), containers};
}
template <typename T> ResultJSON DockerClient<T>::get_info()
{
  auto response = client->get("/info");
  auto info = json::parse(response.data());
  return {response.status(), info};
}
template <typename T>
ResultJSON
DockerClient<T>::create_container(const std::string &image_name,
                                  const std::vector<std::string> &commands,
                                  const std::vector<std::string> &binds)
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
                    {"HostConfig", {{"Binds", binds}}},
                    {"WorkingDir", "/home/sandbox"}};
  const auto request_data = container.dump();
  logger->debug("Container creation JSON: {}", container.dump(4));
  auto response = client->post("/containers/create", request_data);
  logger->debug("Create container: {0}", response.dump());

  if (response.status() == HTTPStatus::CREATED) {
    auto data_json = json::parse(response.data());
    return {response.status(), data_json};
  }
  throw cannot_create_container_exception{response};
}

template <typename T>
bool DockerClient<T>::start_container(const Container &container)
{
  auto request = fmt::format("/containers/{0}/start", container.id);
  auto response = client->post(request);
  logger->debug("Start container: {0}", response.dump());
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

template <typename T>
Result<std::string, std::string>
DockerClient<T>::attach_container(const Container &container)
{
  auto request = fmt::format(
      "/containers/{0}/attach?stream=1&logs=1&stdout=1&stderr=1", container.id);
  boost::asio::streambuf output;
  boost::asio::streambuf error;
  client->set_output_stream(&output);
  client->set_error_stream(&error);
  client->use_output_streams(true);

  auto response = while_do(
      [&]() {
        return client->post(request, "", {{"Upgrade", "tcp"}});
      },
      [&]() {
        // std::string data{std::istreambuf_iterator<char>(&output), {}};
        // if (!data.empty()) {
        // logger->info("Attach container: {0}", data);
        //}
        // std::string data_error{std::istreambuf_iterator<char>(&error), {}};
        // if (!data_error.empty()) {
        // logger->error("Attach container: {0}", data_error);
        //}
      });

  std::string data_error{std::istreambuf_iterator<char>(&error), {}};
  std::string data{std::istreambuf_iterator<char>(&output), {}};
  logger->debug("Attach container: {0}", data);
  logger->debug("Attach container (error): {0}", data_error);
  client->use_output_streams(false);
  return {response.status(), data, data_error};
}

template <typename T>
ResultJSON DockerClient<T>::inspect_container(const Container &container)
{
  auto query_path = fmt::format("/containers/{0}/json", container.id);
  auto response = client->get(query_path);

  auto data = json::parse(response.data());
  logger->debug("Inspect container: {0}", data.dump(2));
  return {response.status(), data};
}
template <typename T>
ResultJSON DockerClient<T>::remove_container(const Container &container)
{
  auto query_path = fmt::format("/containers/{0}", container.id);
  auto response = client->request(HTTPMethod::DELETE, query_path);
  logger->debug("Remove container: {0}", container.id);
  if (response.data().empty()) {
    return {response.status(), json{}};
  }
  else {
    auto data = json::parse(response.data());
    return {response.status(), data};
  }
}

template <typename T> void DockerClient<T>::run()
{
  // auto container = createContainer(
  // image_name,
  //{"/bin/zsh", "-c",
  //"count=1; repeat 2 { echo $count && sleep 1; (( count++ )) } "},
  //{"/tmp:/home/tmp"});

  auto mount_dir = fs::canonical("../data/test_sandbox");
  auto container_json = std::get<1>(create_container(
      image_name,
      {"/bin/zsh", "-c",
       "count=1; repeat 2 { echo $count && sleep 1; (( count++ )) } "},
      //{"/home/sandbox/main"},
      //{"g++", "main.cpp"},
      {fmt::format("{}:{}", mount_dir.string(), "/home/sandbox")}));

  Container container{container_json["Id"]};
  // auto container = createContainer(image_name, {"date"});
  // inspectContainer(container);
  start_container(container);
  attach_container(container);
   inspect_container(container);
  remove_container(container);
  // inspect_container(container);
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
