
#include "utils.h"


#include <Poco/StreamCopier.h>
#include <algorithm>
#include <sstream>

namespace Sphinx {

std::string to_string(Poco::Pipe &pipe) {
    Poco::PipeInputStream istr(pipe);
    std::string output;
    Poco::StreamCopier::copyToString(istr, output);
    return output;
}

} // namespace Sphinx

namespace std {

std::ostream& operator<< (std::ostream& out, const boost::program_options::variable_value& v)
{
    return out;
}
} // namespace std
