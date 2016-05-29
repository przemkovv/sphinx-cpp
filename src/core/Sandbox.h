
#pragma once

#include <string>
#include <vector>
#include <map>

#include <Poco/Logger.h>
#include <Poco/Path.h>
#include <Poco/File.h>

#include "File.h"

namespace Sphinx {


class Sandbox {
    public:
        Sandbox();
        Sandbox(std::initializer_list<File> files);
        ~Sandbox() = default;
        void addFile(File file);

        template<class C>
        void addFiles(const C& files);

        void copyFile(Poco::Path source, FileType file_type);

        Poco::Path getProjectRootPath() { return project_root_path; }

        const char *name() const { return "Sphinx::Sandbox"; }

        const std::vector<File>& getFiles() const { return files; }

    protected:
    private:
        /* data */
        Poco::Path project_root_path;
        Poco::Path project_src_path;
        std::vector<File> files;
        Poco::Logger& logger;

        void addFile(File file, Poco::Path destination);
        Poco::Path getDestinationPath(FileType file_type);

};
}
