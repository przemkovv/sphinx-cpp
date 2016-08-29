
#include "DockerStatus.h"

namespace Sphinx::Docker::v2 {

template <>
DockerStatus translate_status<DockerOperation::CreateContainer>(
    const HTTPStatus &http_status)
{
  auto status = DockerStatus::UndefinedError;
  if (http_status == HTTPStatus::CREATED) {
    status = DockerStatus::NoError;
  }
  else if (http_status == HTTPStatus::BAD_REQUEST) {
    status = DockerStatus::BadParameter;
  }
  else if (http_status == HTTPStatus::NOT_FOUND) {
    status = DockerStatus::NoSuchContainer;
  }
  else if (http_status == HTTPStatus::NOT_ACCEPTABLE) {
    status = DockerStatus::ImpossibleToAttach;
  }
  else if (http_status == HTTPStatus::SERVER_ERROR) {
    status = DockerStatus::ServerError;
  }
  return status;
}

template <>
DockerStatus translate_status<DockerOperation::AttachContainer>(
    const HTTPStatus &http_status)
{

  auto status = DockerStatus::UndefinedError;
  if (http_status == HTTPStatus::SWITCHING_PROTOCOLS ||
      http_status == HTTPStatus::OK) {
    status = DockerStatus::NoError;
  }
  else if (http_status == HTTPStatus::BAD_REQUEST) {
    status = DockerStatus::BadParameter;
  }
  else if (http_status == HTTPStatus::NOT_FOUND) {
    status = DockerStatus::NoSuchContainer;
  }
  else if (http_status == HTTPStatus::SERVER_ERROR) {
    status = DockerStatus::ServerError;
  }
  return status;
}
template <>
DockerStatus translate_status<DockerOperation::InspectContainer>(
    const HTTPStatus &http_status)
{
  auto status = DockerStatus::NoError;
  if (http_status == HTTPStatus::NOT_FOUND) {
    status = DockerStatus::NoSuchContainer;
  }
  else if (http_status == HTTPStatus::SERVER_ERROR) {
    status = DockerStatus::ServerError;
  }
  return status;
}
template <>
DockerStatus translate_status<DockerOperation::RemoveContainer>(
    const HTTPStatus &http_status)
{
  auto status = DockerStatus::NoError;
  if (http_status == HTTPStatus::OK) {
    status = DockerStatus::NoError;
  }
  else if (http_status == HTTPStatus::BAD_REQUEST) {
    status = DockerStatus::BadParameter;
  }
  else if (http_status == HTTPStatus::NOT_FOUND) {
    status = DockerStatus::NoSuchContainer;
  }
  else if (http_status == HTTPStatus::SERVER_ERROR) {
    status = DockerStatus::ServerError;
  }
  return status;
}
template <>
DockerStatus
translate_status<DockerOperation::WaitContainer>(const HTTPStatus &http_status)
{

  auto status = DockerStatus::UndefinedError;
  if (http_status == HTTPStatus::OK) {
    status = DockerStatus::NoError;
  }
  else if (http_status == HTTPStatus::NOT_FOUND) {
    status = DockerStatus::NoSuchContainer;
  }
  else if (http_status == HTTPStatus::SERVER_ERROR) {
    status = DockerStatus::ServerError;
  }
  return status;
}
template <>
DockerStatus
translate_status<DockerOperation::StopContainer>(const HTTPStatus &http_status)
{
  auto status = DockerStatus::NoError;
  if (http_status == HTTPStatus::NO_CONTENT) {
    status = DockerStatus::NoError;
  }
  else if (http_status == HTTPStatus::SEE_OTHER) {
    status = DockerStatus::ContainerAlreadyStopped;
  }
  else if (http_status == HTTPStatus::NOT_FOUND) {
    status = DockerStatus::NoSuchContainer;
  }
  else if (http_status == HTTPStatus::SERVER_ERROR) {
    status = DockerStatus::ServerError;
  }
  return status;
}
template <>
DockerStatus
translate_status<DockerOperation::StartContainer>(const HTTPStatus &http_status)
{

  auto status = DockerStatus::UndefinedError;
  if (http_status == HTTPStatus::NO_CONTENT) {
    status = DockerStatus::NoError;
  }
  else if (http_status == HTTPStatus::SEE_OTHER) {
    status = DockerStatus::ContainerAlreadyStarted;
  }
  else if (http_status == HTTPStatus::NOT_FOUND) {
    status = DockerStatus::NoSuchContainer;
  }
  else if (http_status == HTTPStatus::SERVER_ERROR) {
    status = DockerStatus::ServerError;
  }
  return status;
}
} // namespace Sphinx::Docker::v2
