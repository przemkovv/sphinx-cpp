
#include "Sandbox.h"

#include <string>
#include <exception>
#include <fstream>

#include "utils.h"

namespace Sphinx {

namespace fs = boost::filesystem;

Sandbox::Sandbox(): logger_(make_logger(name()))
{
    using namespace std::string_literals;

    project_root_path_ = fs::unique_path(fs::temp_directory_path() / "sphinx"s / "%%%%%%%"s);
    create_directory(project_root_path_);
    
    project_src_path_ = project_root_path_ / "src"s;
    create_directory(project_src_path_);

    project_bin_path_ = project_root_path_ / "bin"s;
    create_directory(project_bin_path_);

    output_executable_path_ = project_bin_path_ / "run"s;
}

void Sandbox::create_directory(const fs::path &path) {
    logger_->info("Creating temporary dir: {}", path.string());
    fs::create_directories(path);

    if (!exists(path)) {
        throw std::invalid_argument(fmt::format("Cannot create directory: {}", path.string()));
    }
}

Sandbox::Sandbox(std::initializer_list<File> files)
    : Sandbox()
{
    add_files(files);
}

template<class C>
void Sandbox::add_files(const C& files)
{
    for (const auto& file : files) {
        add_file(file);
    }
}

void Sandbox::add_file(File file)
{
    add_file(file, get_destination_path(file.file_type));
}
void Sandbox::add_file(File file, fs::path destination)
{
    file.full_path = destination/ file.name;
    std::ofstream filestream(file.full_path.string(), std::ofstream::out);
    filestream << file.content;
    filestream.close();
    files_.emplace_back(std::move(file));
}
void Sandbox::copy_file(fs::path source, FileType file_type)
{
    fs::path file(source);
    fs::copy(file, get_destination_path(file_type));
}
fs::path Sandbox::get_destination_path(FileType file_type)
{
    fs::path destination;

    switch (file_type) {
        case FileType::Source:
        case FileType::Header:
            destination = project_src_path_;
            break;

        case FileType::Other:
            destination = project_root_path_;
            break;
    }

    return destination;
}
} // namespace Sphinx
