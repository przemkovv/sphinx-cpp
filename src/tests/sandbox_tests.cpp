#include "Catch/catch.hpp"

#include "File.h"
#include "Sandbox.h"

namespace fs = boost::filesystem;

SCENARIO("files to the sandbox can be added2", "[sandbox]") {
    GIVEN("An empty sandbox") {
        Sphinx::Sandbox sandbox;
    
        REQUIRE( sandbox.files().size() == 0 );

        WHEN( "the new file is added" ) {
            Sphinx::File source_file {"main.cpp",  R"code(
#include <iostream>
                int main() {
                    std::cout << "Hello World" << std::endl
                        return 0;
                } )code"
            };

            sandbox.add_file(source_file);
            THEN ("the files number increase to one and the file with proper name is created") {
                REQUIRE( sandbox.files().size()  == 1);
                REQUIRE( fs::path( sandbox.files()[0].full_path).filename() == source_file.name );
            }
        }
    }
}



