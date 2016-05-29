
#pragma once
#include <string>
#include <map>
#include <Poco/Path.h>

namespace Sphinx {

enum class FileType  {
    Source, Header, Other
};

const std::map <std::string, FileType> extensionFileType  {
    {"cpp", FileType::Source},
    {"c", FileType::Source},
    {"cc", FileType::Source},
    {"hpp", FileType::Header},
    {"h", FileType::Header}};


struct File {
    /* data */
    std::string name;
    std::string content;
    FileType file_type;

    Poco::Path full_path;

    File(std::string name, std::string content);
    File(std::string name, std::string content, FileType file_type);

    FileType getFileType(std::string filename);
};


}
