
#pragma once

#include <string>
#include <vector>
#include <map>

#include <boost/filesystem.hpp>

#include "File.h"
#include "Logger.h"

namespace Sphinx {


class Sandbox {
    public:
        Sandbox();
        Sandbox(std::initializer_list<File> files);
        ~Sandbox() = default;
        void addFile(File file);

        template<class C>
        void addFiles(const C& files);

        void copyFile(boost::filesystem::path source, FileType file_type);

        boost::filesystem::path getProjectRootPath() { return project_root_path; }

        const char *name() const { return "Sphinx::Sandbox"; }

        const std::vector<File>& getFiles() const { return files; }

    protected:
    private:
        /* data */
        boost::filesystem::path project_root_path;
        boost::filesystem::path project_src_path;
        std::vector<File> files;
        Logger logger;

        void addFile(File file, boost::filesystem::path destination);
        boost::filesystem::path getDestinationPath(FileType file_type);


};
}
