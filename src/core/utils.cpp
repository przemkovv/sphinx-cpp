
#include "utils.h"

#include <Poco/StreamCopier.h>

namespace Sphinx {

std::string to_string(Poco::Pipe &pipe) {
    Poco::PipeInputStream istr(pipe);
    std::string output;
    Poco::StreamCopier::copyToString(istr, output);
    return output;
}

std::string escape_control_characters(const std::string &input)
{
  std::string output;
  for (auto currentValue : input) {
    switch (currentValue) {
    case L'\t':
      output.append("\\t");
      break;
    case L'\\':
      output.append("\\\\");
      break;
    case L'\r':
      output.append("\\r");
      break;
    case L'\n':
      output.append("\\n");
      break;
    //.... etc.
    default:
      output.push_back(currentValue);
    }
  }
  return output;
}

} // namespace Sphinx

namespace std {

std::ostream& operator<< (std::ostream& out, const boost::program_options::variable_value& /*v*/)
{
    return out;
}
} // namespace std
