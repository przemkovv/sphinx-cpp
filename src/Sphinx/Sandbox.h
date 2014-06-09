
#pragma once

#include <string>
#include <vector>

#include <Poco/Logger.h>
#include <Poco/Path.h>
#include <Poco/File.h>

#include "Sphinx/File.h"

namespace Sphinx {
class Sandbox {
    public:
        Sandbox();
        ~Sandbox();
        void addFile(const File& file);

        template<class C>
        void addFiles(const C& files)
        {
            for (const auto& file : files) {
                addFile(file);
            }
        }

        const char *name() const
        {
            return "Sphinx::Sandbox";
        }

    protected:
        void prepareEnvironment();
        std::string makeTemporaryDir() ;
    private:
        /* data */
        Poco::Path tmp_path;
        std::vector<Poco::File> files;
        Poco::Logger& logger;

};
}
