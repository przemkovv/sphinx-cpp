
#include "SampleData.h"


namespace Sphinx {
namespace SampleData {

Sandbox simpleHelloWorld()
{
    return {
        {
            "foo.cpp",  R"code(
#include <iostream>
int foo() {
    std::cout << "int foo(){}" << std::endl;
    return 0;
} )code"

        },
        {
            "foo.h",  R"code(
int foo();
 )code"
        },
        {
            "main.cpp",  R"code(
#include <iostream>
#include "foo.h"
int main() {
    std::cout << "Hello World" << std::endl;
    foo();
    return 0
} )code"
        }};
}

} // namespace SampleData
} // namespace Sphinx