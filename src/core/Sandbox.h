
#pragma once

#include <string>
#include <vector>

#include <Poco/Logger.h>
#include <Poco/Path.h>
#include <Poco/File.h>

#include "File.h"

namespace Sphinx {

enum class FileType  {
    Source, Other
};

class Sandbox {
    public:
        Sandbox();
        ~Sandbox();
        void addFile(const File& file, const FileType file_type = FileType::Source);

        template<class C>
        void addFiles(const C& files, const FileType file_type = FileType::Source)
        {
            for (const auto& file : files) {
                addFile(file, file_type);
            }
        }
        void copyFile(Poco::Path source, FileType file_type);

        Poco::Path getProjectRootPath() { return project_root_path; }

        const char *name() const
        {
            return "Sphinx::Sandbox";
        }

        const std::vector<Poco::File>& getFiles() const{
            return files;
        }

    protected:
    private:
        /* data */
        Poco::Path project_root_path;
        Poco::Path project_src_path;
        std::vector<Poco::File> files;
        Poco::Logger& logger;

        void addFile(const File& file, Poco::Path destination);
        Poco::Path  getDestinationPath(FileType file_type);

};
}
