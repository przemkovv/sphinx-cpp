
#include "SampleData.h"

namespace Sphinx {
namespace SampleData {

Sandbox simple_hello_world()
{
  return {{"foo.cpp", R"code(
#include <iostream>
int foo() {
    std::cout << "int foo(){}" << std::endl;
    return 0;
} )code"

          },
          {"foo.h", R"code(
int foo();
 )code"},
          {"main.cpp", R"code(
#include "foo.h"
#include <iostream>
int main() {
    std::cout << "Hello World" << std::endl;
    foo();
    return 0;
} )code"}};
}

Sandbox simple_hello_world_compile_error()
{
  return {{"foo.cpp", R"code(
#include <iostream>
int foo() {
    std::cout << "int foo(){}" << std::endl;
    return 0;
} )code"

          },
          {"foo.h", R"code(
int foo();
 )code"},
          {"main.cpp", R"code(
#include "foo.h"
#include <iostream>
int main() {
    std::cout << "Hello World" << std::endl;
    foo();
    return 0
} )code"}};
}

} // namespace SampleData
} // namespace Sphinx
