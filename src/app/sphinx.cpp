
#include "Application.h"

#include <iostream>
#include <stdexcept>

int main(int argc, char *argv[])
{
    Sphinx::Application sphinx{{argv + 1, argv + argc}};
    return sphinx.run();
}
