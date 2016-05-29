
#include "utils.h"


#include <Poco/StreamCopier.h>

namespace Sphinx {
Poco::Path operator+(Poco::Path path, std::string segment)
{
    path.append(segment);
    return path;
}

std::string to_string(Poco::Pipe &pipe) {
    Poco::PipeInputStream istr(pipe);
    std::string output;
    Poco::StreamCopier::copyToString(istr, output);
    return output;
}


} // Sphinx
