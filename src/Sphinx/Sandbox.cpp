
#include "Sandbox.h"

#include <string>
#include <Poco/Path.h>
#include <Poco/TemporaryFile.h>
#include <fstream>

namespace Sphinx {

Sandbox::Sandbox(): logger(Poco::Logger::get(name()))
{
    tmp_path = Poco::TemporaryFile::tempName();
    tmp_path.makeDirectory();
    logger.information("Creating temporary dir: " + tmp_path.toString());
    Poco::File(tmp_path).createDirectories();
}

Sandbox::~Sandbox()
{
}

void Sandbox::addFile(const File& file)
{
    Poco::File f(Poco::Path(tmp_path, file.name));
    std::ofstream filestream(f.path(), std::ofstream::out);
    filestream << file.content;
    filestream.close();
    
}
void Sandbox::prepareEnvironment() {}
std::string Sandbox::makeTemporaryDir()
{
    return "";
}
}
