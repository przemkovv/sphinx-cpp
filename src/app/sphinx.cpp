
#include <stdexcept>
#include <iostream>

#include <Poco/Util/OptionException.h>
#include "Application.h"

int main(int argc, char *argv[])
{
    try {
        Sphinx::Application sphinx{{argv+1, argv+argc}};
        //sphinx.init(argc, argv);
        return sphinx.run();
    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
        return -1;
    }
}
