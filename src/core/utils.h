
#pragma once

#include "pprint.hpp"

#include <boost/program_options.hpp>
#include <Poco/PipeStream.h>

#include <iostream>
#include <string>

namespace Sphinx {

enum class ExitCode
/// Commonly used exit status codes.
/// Based on the definitions in the 4.3BSD <sysexits.h> header file.
{
    OK          = 0,  /// successful termination
    USAGE	    = 64, /// command line usage error
    DATAERR     = 65, /// data format error
    NOINPUT     = 66, /// cannot open input
    NOUSER      = 67, /// addressee unknown
    NOHOST      = 68, /// host name unknown
    UNAVAILABLE = 69, /// service unavailable
    SOFTWARE    = 70, /// internal software error
    OSERR	    = 71, /// system error (e.g., can't fork)
    OSFILE      = 72, /// critical OS file missing
    CANTCREAT   = 73, /// can't create (user) output file
    IOERR       = 74, /// input/output error
    TEMPFAIL    = 75, /// temp failure; user is invited to retry
    PROTOCOL    = 76, /// remote error in protocol
    NOPERM      = 77, /// permission denied
    CONFIG      = 78  /// configuration error
};

std::string to_string(Poco::Pipe& pipe);

/// @brief Auxiliary function to create a shared unique ptr.
template<class T, class...Args >
std::shared_ptr<std::unique_ptr<T>> make_shared_unique(Args&&... args)
{
  return std::make_shared<std::unique_ptr<T>>(
      std::make_unique<T>(std::forward<Args>(args)...));
}
/// @brief Auxiliary function to create a shared unique ptr.
template<class T, class...Args >
std::shared_ptr<std::unique_ptr<T>> make_shared_unique_from_unique(std::unique_ptr<T> ptr)
{
  return std::make_shared<std::unique_ptr<T>>(ptr);
}


} //namespace Sphinx

namespace std {

std::ostream& operator<< (std::ostream& out, const boost::program_options::variable_value& v);

} // namespace std

