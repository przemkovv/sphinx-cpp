
#include "File.h"
#include "Logger.h"

namespace Sphinx {

File::File(std::string name_, std::string content_)
  : File(name_, content_, get_file_type(name_))
{
}

File::File(std::string name_, std::string content_, FileType file_type_)
  : name(name_), content(content_), file_type(file_type_)
{
}
FileType File::get_file_type(std::string filename)
{
  return extensionFileType.at(
      boost::filesystem::path(filename).extension().string());
}

} // Sphinx
