
#include "Catch/catch.hpp"

#include "Compilers/MakeCompiler.h"


SCENARIO("MakeCompiler return correct values when program is correct and incorrect", "[MakeCompiler]")
{
    GIVEN("A compiler using Makefile") {
        Sphinx::Compilers::MakeCompiler compiler;
        WHEN(" project is compilable") {
            Sphinx::File file {"main.cpp",  R"code(
#include <iostream>
int main() {
    std::cout << "Hello World" << std::endl;
    return 0;
} )code"
                              };
            Sphinx::Sandbox sandbox;
            sandbox.add_file(file);
            THEN("compiler returns no errors") {
                REQUIRE(compiler.compile(sandbox) == true);
                REQUIRE(compiler.get_errors().size() == 0);
                REQUIRE(compiler.get_errors().empty() == true);
            }
        }

        WHEN(" project has errors") {
            Sphinx::File file {"main.cpp",  R"code(
#include <iostream>
int main() {
    std::cout << "Hello World" << std::endl
    return 0;
} )code"
                              };
            Sphinx::Sandbox sandbox;
            sandbox.add_file(file);
            THEN("compiler returns errors") {
                REQUIRE(compiler.compile(sandbox) == false);
                REQUIRE(compiler.get_errors().empty() == false);
            }
        }
    }
}
