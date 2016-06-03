
#include "Sandbox.h"

#include <string>
#include <exception>
#include <fstream>
#include <Poco/Path.h>
#include <Poco/TemporaryFile.h>

#include "utils.h"

namespace Sphinx {


Sandbox::Sandbox(): logger(make_logger(name()))
{
    using namespace std::string_literals;
    project_root_path = Poco::TemporaryFile::tempName(Poco::Path::temp() + "sphinx"s);
    project_root_path.makeDirectory();
    project_src_path = project_root_path + "src"s;
    logger->info("Creating temporary dir: {}", project_root_path.toString());
    Poco::File(project_root_path).createDirectories();

    if (!Poco::File(project_root_path).exists()) {
        throw std::invalid_argument(fmt::format("Cannot create directory: {} ", project_root_path.toString()));
    }

    logger->info("Creating temporary dir: {}", project_src_path.toString());
    Poco::File(project_src_path).createDirectories();

    if (!Poco::File(project_src_path).exists()) {
        throw std::invalid_argument(fmt::format("Cannot create directory: {}", project_src_path.toString()));
    }
}

Sandbox::Sandbox(std::initializer_list<File> files)
    : Sandbox()
{
    addFiles(files);
}

template<class C>
void Sandbox::addFiles(const C& files)
{
    for (const auto& file : files) {
        addFile(file);
    }
}

void Sandbox::addFile(File file)
{
    addFile(file, getDestinationPath(file.file_type));
}
void Sandbox::addFile(File file, Poco::Path destination)
{
    file.full_path = Poco::Path(destination, file.name);
    std::ofstream filestream(file.full_path.toString(), std::ofstream::out);
    filestream << file.content;
    filestream.close();
    files.emplace_back(std::move(file));
}
void Sandbox::copyFile(Poco::Path source, FileType file_type)
{
    Poco::File file(source);
    file.copyTo(getDestinationPath(file_type).toString());
}
Poco::Path Sandbox::getDestinationPath(FileType file_type)
{
    Poco::Path destination;

    switch (file_type) {
        case FileType::Source:
        case FileType::Header:
            destination = project_src_path;
            break;

        case FileType::Other:
            destination = project_root_path;
            break;
    }

    return destination;
}
} // namespace Sphinx
