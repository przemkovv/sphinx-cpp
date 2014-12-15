
#include <string>
#include <Poco/Path.h>

namespace Sphinx {

	enum class ExitCode
		/// Commonly used exit status codes.
		/// Based on the definitions in the 4.3BSD <sysexits.h> header file.
	{
		OK          = 0,  /// successful termination
		USAGE	     = 64, /// command line usage error
		DATAERR     = 65, /// data format error
		NOINPUT     = 66, /// cannot open input
		NOUSER      = 67, /// addressee unknown
		NOHOST      = 68, /// host name unknown
		UNAVAILABLE = 69, /// service unavailable
		SOFTWARE    = 70, /// internal software error
		OSERR	     = 71, /// system error (e.g., can't fork)
		OSFILE      = 72, /// critical OS file missing
		CANTCREAT   = 73, /// can't create (user) output file
		IOERR       = 74, /// input/output error
		TEMPFAIL    = 75, /// temp failure; user is invited to retry
		PROTOCOL    = 76, /// remote error in protocol
		NOPERM      = 77, /// permission denied
		CONFIG      = 78  /// configuration error
	};

Poco::Path operator+(const Poco::Path& path, std::string segment) {
    Poco::Path new_path = path;
    new_path.append(segment);
    return new_path;
}

}
