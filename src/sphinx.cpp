
#include <iostream>
#include <Poco/Util/OptionException.h>
#include "Sphinx/Application.h"

int main(int argc, char *argv[])
{
    try {
        Sphinx::Application sphinx;
        sphinx.init(argc, argv);
        return sphinx.run();
    } catch (Poco::Util::UnknownOptionException& e) {
        std::cout << e.name() << ": "  << e.message() << std::endl;
        return -1;
    }
}
