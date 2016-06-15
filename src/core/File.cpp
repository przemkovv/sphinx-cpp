
#include "File.h"
#include "Logger.h"


namespace Sphinx {


File::File(std::string name, std::string content)
    : File(name, content, getFileType(name))
{
}

File::File(std::string name, std::string content, FileType file_type)
    : name(name), content(content), file_type(file_type)
{
}
FileType File::getFileType(std::string filename)
{
    return extensionFileType.at(boost::filesystem::path(filename).extension().string());
}


} //Sphinx
