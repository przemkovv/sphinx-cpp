
#include "DockerClient.h"

#include "DockerExceptions.h"

#include "HTTPCommon.h"
#include <fmt/format.h>

#include <chrono>
#include <future>
#include <thread>

namespace Sphinx {
namespace Docker {
namespace v2 {

using json = nlohmann::json;
namespace fs = boost::filesystem;

template <typename T> ResultJSON DockerSocketClient<T>::list_images()
{
  auto response = client->get("/images/json?all=1");
  auto images = json::parse(response.data());

  return {DockerStatus::NoError, images};
}
template <typename T> ResultJSON DockerSocketClient<T>::list_containers()
{
  auto response = client->get("/containers/json?all=1&size=1");
  auto containers = json::parse(response.data());

  return {DockerStatus::NoError, containers};
}
template <typename T> ResultJSON DockerSocketClient<T>::get_info()
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
ResultJSON DockerSocketClient<T>::create_container(
    const std::string &image_name,
    const std::vector<std::string> &commands,
    const fs::path &working_dir,
    const std::vector<std::pair<fs::path, fs::path>> &mounting_points)
{

  std::vector<std::string> binds;
  std::transform(std::begin(mounting_points), std::end(mounting_points),
                 std::back_inserter(binds), [](const auto &p) {
                   return fmt::format("{0}:{1}", p.first.string(),
                                      p.second.string());
                 });

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
                    {"WorkingDir", working_dir.string()}};
  const auto request_data = container.dump();
  logger->debug("Container creation JSON: {}", container.dump(4));
  auto response = client->post("/containers/create", request_data);
  logger->debug("Create container: {0}", response.dump());

  auto status =
      translate_status<DockerOperation::CreateContainer>(response.status());
  return {status, json::parse(response.data())};
}

template <typename T>
ResultJSON DockerSocketClient<T>::wait_container(const Container &container)
{
  auto request = fmt::format("/containers/{0}/wait", container.id);
  auto response = client->post(request);
  logger->debug("Waiting for the container: {0}", response.dump());

  auto status =
      translate_status<DockerOperation::WaitContainer>(response.status());
  return {status, json::parse(response.data())};
}

template <typename T>
ResultJSON DockerSocketClient<T>::start_container(const Container &container)
{
  auto request = fmt::format("/containers/{0}/start", container.id);
  auto response = client->post(request);
  logger->debug("Start container: {0}", response.dump());

  auto status =
      translate_status<DockerOperation::StartContainer>(response.status());

  if (response.data().empty()) {
    return {status, json{}};
  }
  return {status, json::parse(response.data())};
}

template <typename T>
ResultJSON DockerSocketClient<T>::attach_container(const Container &container,
                                                   IOBuffers &io_buffers)
{
  auto request = fmt::format(
      "/containers/{0}/attach?stream=1&logs=1&stdout=1&stderr=1&stdin=1",
      container.id);

  client->set_output_stream(&io_buffers.output);
  client->set_error_stream(&io_buffers.error);
  client->set_input_stream(&io_buffers.input);
  client->use_output_streams(true);
  client->use_input_stream(true);

  auto response = client->post(request, "", {{"Upgrade", "tcp"}});

  client->use_output_streams(false);
  client->use_input_stream(false);

  auto status =
      translate_status<DockerOperation::AttachContainer>(response.status());

  return {status, {}};
}

template <typename T>
ResultJSON DockerSocketClient<T>::inspect_container(const Container &container)
{
  auto query_path = fmt::format("/containers/{0}/json", container.id);
  auto response = client->get(query_path);

  logger->debug("Inspect container: {0}", response.data());

  auto status =
      translate_status<DockerOperation::InspectContainer>(response.status());
  return {status, json::parse(response.data())};
}
template <typename T>
ResultJSON DockerSocketClient<T>::remove_container(const Container &container)
{
  auto query_path = fmt::format("/containers/{0}", container.id);
  auto response = client->request(HTTPMethod::DELETE, query_path);
  logger->debug("Remove container: {0}", container.id);

  auto status =
      translate_status<DockerOperation::RemoveContainer>(response.status());
  if (response.data().empty()) {
    return {status, json{}};
  }
  return {status, json::parse(response.data())};
}

template <typename T>
ResultJSON DockerSocketClient<T>::stop_container(const Container &container,
                                                 unsigned int wait_time)
{
  auto query_path =
      fmt::format("/containers/{0}/stop?t={1}", container.id, wait_time);
  auto response = client->post(query_path);
  logger->debug("Stop container: {0}", container.id);

  auto status =
      translate_status<DockerOperation::StopContainer>(response.status());
  if (response.data().empty()) {
    return {status, json{}};
  }
  return {status, json::parse(response.data())};
}
template <typename T>
int DockerSocketClient<T>::run_command_in_mounted_dir(
    const std::vector<std::string> &cmd,
    const fs::path &mount_dir,
    IOBuffers &io_buffers)
{
  auto working_dir = fs::path("/home/sandbox");
  auto create_result = create_container(
      image_name, cmd, working_dir, {std::make_pair(mount_dir, working_dir)});
  throw_if_error(create_result);

  Container container{std::get<1>(create_result).at("Id")};
  SCOPE_EXIT(throw_if_error(remove_container(container)));

  auto start_result = start_container(container);
  throw_if_error(start_result);

  auto attach_result = attach_container(container, io_buffers);
  throw_if_error(attach_result);

  auto wait_result = wait_container(container);
  throw_if_error(wait_result);

  int exit_code = std::get<1>(wait_result).at("StatusCode");

  return exit_code;
}

template <typename T>
std::tuple<std::string, std::string, int>
DockerSocketClient<T>::run_command_in_mounted_dir(
    const std::vector<std::string> &cmd,
    const fs::path &mount_dir,
    const std::string &stdin)
{
  IOBuffers io_buffers;
  std::ostream stdin_stream(&io_buffers.input);
  stdin_stream << stdin;

  auto exit_code = run_command_in_mounted_dir(cmd, mount_dir, io_buffers);

  std::string data_error{std::istreambuf_iterator<char>(&io_buffers.error), {}};
  std::string data{std::istreambuf_iterator<char>(&io_buffers.output), {}};

  return {data, data_error, exit_code};
}

template <typename T>
template <typename U>
std::string DockerSocketClient<T>::get_message_error(const U & /*data*/)
{
  return {};
}
template <typename T>
std::string DockerSocketClient<T>::get_message_error(const nlohmann::json &data)
{
  return data["message"];
}
template <typename T>
template <typename... U>
bool DockerSocketClient<T>::throw_if_error(const Result<U...> &result)
{
  const auto &status = std::get<0>(result);
  if (status != DockerStatus::NoError) {
    std::string message = get_message_error(std::get<1>(result));
    logger->error("Status code: {}, Error message: {}",
                  static_cast<int>(status), message);
    throw docker_exception{message};
  }
  return false;
}
template <typename T>
template <typename... U>
bool DockerSocketClient<T>::is_error(const Result<U...> &result)
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

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wweak-template-vtables"

template class DockerSocketClient<UnixSocket>;
template class DockerSocketClient<TCPSocket>;

#pragma clang diagnostic pop

} // namespace v2
} // namespace Docker
} // namespace Sphinx
