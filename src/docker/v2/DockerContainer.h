#pragma once

#include <string>

namespace Sphinx {
namespace Docker {
namespace v2 {


struct Container {
  std::string id;
};

struct Image {
  std::string name;
};

} // namespace v2
} // namespace Docker
} // namespace Sphinx
