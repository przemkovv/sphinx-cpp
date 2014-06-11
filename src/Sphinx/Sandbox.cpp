
#include "Sandbox.h"

#include <string>
#include <Poco/Path.h>
#include <Poco/TemporaryFile.h>
#include <fstream>

namespace Sphinx {

Sandbox::Sandbox(): logger(Poco::Logger::get(name()))
{
    project_root_path = Poco::TemporaryFile::tempName();
    project_root_path.makeDirectory();
    project_src_path = project_root_path;
    project_src_path.pushDirectory("src");
    logger.information("Creating temporary dir: " + project_root_path.toString());
    Poco::File(project_root_path).createDirectories();
    logger.information("Creating temporary dir: " + project_src_path.toString());
    Poco::File(project_src_path).createDirectories();
}

Sandbox::~Sandbox()
{
}

void Sandbox::addFile(const File& file, const FileType file_type)
{
    addFile(file, getDestinationPath(file_type));
}
void Sandbox::addFile(const File& file, Poco::Path destination)
{
    Poco::File f(Poco::Path(destination, file.name));
    std::ofstream filestream(f.path(), std::ofstream::out);
    filestream << file.content;
    filestream.close();
}
void Sandbox::copyFile(Poco::Path source, FileType file_type)
{
    Poco::File file(source);
    file.copyTo(getDestinationPath(file_type).toString());
}
Poco::Path  Sandbox::getDestinationPath(FileType file_type)
{
    Poco::Path destination;

    switch (file_type) {
        case FileType::Source:
            destination = project_src_path;
            break;

        case FileType::Other:
            destination = project_root_path;
            break;
    }
    return destination;
}
}
