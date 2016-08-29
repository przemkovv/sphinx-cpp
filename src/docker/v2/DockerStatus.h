
#pragma once

#include "HTTPCommon.h"

#include <cassert>

namespace Sphinx::Docker::v2 {

enum class DockerStatus {
  NoError = 0,
  BadParameter,
  ServerError,
  NoSuchContainer,
  ImpossibleToAttach,
  ContainerAlreadyStarted,
  ContainerAlreadyStopped,
  UndefinedError
};

enum class DockerOperation {
  AttachContainer,
  InspectContainer,
  RemoveContainer,
  WaitContainer,
  StopContainer,
  StartContainer,
  CreateContainer
};

template <DockerOperation Operation>
DockerStatus translate_status(const HTTPStatus & /*http_status*/)
{
  assert(sizeof(Operation) == 0 &&
         "Translate status not defined for this operation");
  return DockerStatus::UndefinedError;
}

template <>
DockerStatus translate_status<DockerOperation::CreateContainer>(
    const HTTPStatus &http_status);

template <>
DockerStatus translate_status<DockerOperation::AttachContainer>(
    const HTTPStatus &http_status);
template <>
DockerStatus translate_status<DockerOperation::InspectContainer>(
    const HTTPStatus &http_status);
template <>
DockerStatus translate_status<DockerOperation::RemoveContainer>(
    const HTTPStatus &http_status);
template <>
DockerStatus translate_status<DockerOperation::WaitContainer>(
    const HTTPStatus &http_status);
template <>
DockerStatus translate_status<DockerOperation::StopContainer>(
    const HTTPStatus &http_status);
template <>
DockerStatus translate_status<DockerOperation::StartContainer>(
    const HTTPStatus &http_status);

} // namespace Sphinx::Docker::v2
