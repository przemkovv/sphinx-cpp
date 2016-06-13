
#include "Sandbox.h"

#include <string>
#include <exception>
#include <fstream>

#include "utils.h"

namespace Sphinx {

namespace fs = boost::filesystem;

Sandbox::Sandbox(): logger(make_logger(name()))
{
    using namespace std::string_literals;
    project_root_path = fs::unique_path(fs::temp_directory_path() / "sphinx"s / "%%%%%%%"s);
    fs::create_directories(project_root_path);
    logger->info("Creating temporary dir: {}", project_root_path.string());

    if (!fs::exists(project_root_path)) {
        throw std::invalid_argument(fmt::format("Cannot create directory: {} ", project_root_path.string()));
    }

    logger->info("Creating temporary dir: {}", project_src_path.string());
    project_src_path = project_root_path / "src"s;
    fs::create_directories(project_src_path);

    if (!exists(project_src_path)) {
        throw std::invalid_argument(fmt::format("Cannot create directory: {}", project_src_path.string()));
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
void Sandbox::addFile(File file, fs::path destination)
{
    file.full_path = destination/ file.name;
    std::ofstream filestream(file.full_path.string(), std::ofstream::out);
    filestream << file.content;
    filestream.close();
    files.emplace_back(std::move(file));
}
void Sandbox::copyFile(fs::path source, FileType file_type)
{
    fs::path file(source);
    fs::copy(file, getDestinationPath(file_type));
}
fs::path Sandbox::getDestinationPath(FileType file_type)
{
    fs::path destination;

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
