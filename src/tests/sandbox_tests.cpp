#include "Catch/catch.hpp"

#include <Poco/Path.h>
#include "File.h"
#include "Sandbox.h"


SCENARIO("files to the sandbox can be added2", "[sandbox]") {
    GIVEN("An empty sandbox") {
        Sphinx::Sandbox sandbox;
    
        REQUIRE( sandbox.getFiles().size() == 0 );

        WHEN( "the new file is added" ) {
            Sphinx::File source_file {"main.cpp",  R"code(
#include <iostream>
                int main() {
                    std::cout << "Hello World" << std::endl
                        return 0;
                } )code"
            };

            sandbox.addFile(source_file);
            THEN ("the files number increase to one and the file with proper name is created") {
                REQUIRE( sandbox.getFiles().size()  == 1);
                REQUIRE( Poco::Path( sandbox.getFiles()[0].full_path).getFileName() == source_file.name );
            }
        }
    }
}



