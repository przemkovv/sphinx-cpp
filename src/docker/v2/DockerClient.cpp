
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

  return {DockerStatus::NoError, images};
}
template <typename T> ResultJSON DockerClient<T>::list_containers()
{
  auto response = client->get("/containers/json?all=1&size=1");
  auto containers = json::parse(response.data());

  return {DockerStatus::NoError, containers};
}
template <typename T> ResultJSON DockerClient<T>::get_info()
{
  auto response = client->get("/info");
  auto info = json::parse(response.data());

  auto status = DockerStatus::NoError;
  if (response.status() == HTTPStatus::SERVER_ERROR) {
    status = DockerStatus::ServerError;
  }
  return {status, info};
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

  auto status = DockerStatus::UndefiniedError;
  if (response.status() == HTTPStatus::CREATED) {
    status = DockerStatus::NoError;
  }
  else if (response.status() == HTTPStatus::BAD_REQUEST) {
    status = DockerStatus::BadParameter;
  }
  else if (response.status() == HTTPStatus::NOT_FOUND) {
    status = DockerStatus::NoSuchContainer;
  }
  else if (response.status() == HTTPStatus::NOT_ACCEPTABLE) {
    status = DockerStatus::ImpossibleToAttach;
  }
  else if (response.status() == HTTPStatus::SERVER_ERROR) {
    status = DockerStatus::ServerError;
  }
  return {status, json::parse(response.data())};
}

template <typename T>
ResultJSON DockerClient<T>::start_container(const Container &container)
{
  auto request = fmt::format("/containers/{0}/start", container.id);
  auto response = client->post(request);
  logger->debug("Start container: {0}", response.dump());

  auto status = DockerStatus::UndefiniedError;
  if (response.status() == HTTPStatus::NO_CONTENT) {
    status = DockerStatus::NoError;
  }
  else if (response.status() == HTTPStatus::SEE_OTHER) {
    status = DockerStatus::ContainerAlreadyStarted;
  }
  else if (response.status() == HTTPStatus::NOT_FOUND) {
    status = DockerStatus::NoSuchContainer;
  }
  else if (response.status() == HTTPStatus::SERVER_ERROR) {
    status = DockerStatus::ServerError;
  }

  if (response.data().empty()) {
    return {status, json{}};
  }
  else {
    return {status, json::parse(response.data())};
  }
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
      [&]() {});

  std::string data_error{std::istreambuf_iterator<char>(&error), {}};
  std::string data{std::istreambuf_iterator<char>(&output), {}};
  logger->debug("Attach container: {0}", data);
  logger->debug("Attach container (error): {0}", data_error);
  client->use_output_streams(false);

  auto status = DockerStatus::UndefiniedError;
  if (response.status() == HTTPStatus::SWITCHING_PROTOCOLS ||
      response.status() == HTTPStatus::OK) {
    status = DockerStatus::NoError;
  }
  else if (response.status() == HTTPStatus::BAD_REQUEST) {
    status = DockerStatus::BadParameter;
  }
  else if (response.status() == HTTPStatus::NOT_FOUND) {
    status = DockerStatus::NoSuchContainer;
  }
  else if (response.status() == HTTPStatus::SERVER_ERROR) {
    status = DockerStatus::ServerError;
  }

  // TODO(przemkovv): what to do if we had an error? How to transfer a received
  // error message?
  return {status, data, data_error};
}

template <typename T>
ResultJSON DockerClient<T>::inspect_container(const Container &container)
{
  auto query_path = fmt::format("/containers/{0}/json", container.id);
  auto response = client->get(query_path);

  logger->debug("Inspect container: {0}", response.data());

  auto status = DockerStatus::NoError;
  if (response.status() == HTTPStatus::NOT_FOUND) {
    status = DockerStatus::NoSuchContainer;
  }
  else if (response.status() == HTTPStatus::SERVER_ERROR) {
    status = DockerStatus::ServerError;
  }
  return {status, json::parse(response.data())};
}
template <typename T>
ResultJSON DockerClient<T>::remove_container(const Container &container)
{
  auto query_path = fmt::format("/containers/{0}", container.id);
  auto response = client->request(HTTPMethod::DELETE, query_path);
  logger->debug("Remove container: {0}", container.id);

  auto status = DockerStatus::NoError;
  if (response.status() == HTTPStatus::OK) {
    status = DockerStatus::NoError;
  }
  else if (response.status() == HTTPStatus::BAD_REQUEST) {
    status = DockerStatus::BadParameter;
  }
  else if (response.status() == HTTPStatus::NOT_FOUND) {
    status = DockerStatus::NoSuchContainer;
  }
  else if (response.status() == HTTPStatus::SERVER_ERROR) {
    status = DockerStatus::ServerError;
  }
  if (response.data().empty()) {
    return {status, json{}};
  }
  else {
    return {status, json::parse(response.data())};
  }
}

template <typename T>
ResultJSON DockerClient<T>::stop_container(const Container &container,
                                           unsigned int wait_time)
{
  auto query_path =
      fmt::format("/containers/{0}/stop?t={1}", container.id, wait_time);
  auto response = client->post(query_path);
  logger->debug("Stop container: {0}", container.id);

  auto status = DockerStatus::NoError;
  if (response.status() == HTTPStatus::NO_CONTENT) {
    status = DockerStatus::NoError;
  }
  else if (response.status() == HTTPStatus::SEE_OTHER) {
    status = DockerStatus::ContainerAlreadyStopped;
  }
  else if (response.status() == HTTPStatus::NOT_FOUND) {
    status = DockerStatus::NoSuchContainer;
  }
  else if (response.status() == HTTPStatus::SERVER_ERROR) {
    status = DockerStatus::ServerError;
  }
  if (response.data().empty()) {
    return {status, json{}};
  }
  else {
    return {status, json::parse(response.data())};
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
  auto start_result = start_container(container);
  if (!is_error(start_result)) {
    attach_container(container);
    inspect_container(container);
    remove_container(container);
  }
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

template <typename T>
template <typename U>
std::string DockerClient<T>::get_message_error(const U &)
{
  return {};
}
template <typename T>
std::string DockerClient<T>::get_message_error(const nlohmann::json &data)
{
  return data["message"];
}
template <typename T> bool DockerClient<T>::is_error(const ResultJSON &result)
{
  const auto &status = std::get<0>(result);

  if (status != DockerStatus::NoError) {
    logger->error("Status code: {}, Error message: {}",
                  static_cast<int>(status),
                  get_message_error(std::get<1>(result)));
    return true;
  }
  return false;
}

template class DockerClient<UnixSocket>;
template class DockerClient<TCPSocket>;

} // namespace v2
} // namespace Docker
} // namespace Sphinx
