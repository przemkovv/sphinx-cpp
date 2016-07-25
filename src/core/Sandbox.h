
#pragma once

#include "File.h"
#include "Logger.h"

#include <boost/filesystem.hpp>

#include <map>
#include <string>
#include <vector>

namespace Sphinx {

class Sandbox {
public:
  Sandbox();
  Sandbox(std::initializer_list<File> files);
  void add_file(File file);

  template <class C> void add_files(const C &files);

  void copy_file(boost::filesystem::path source, FileType file_type);

  boost::filesystem::path project_root_path() const { return project_root_path_; }
  boost::filesystem::path project_executable_path() const
  {
    return output_executable_path_;
  }

  const char *name() const { return "Sphinx::Sandbox"; }

  const std::vector<File> &files() const { return files_; }

protected:
private:
  /* data */
  boost::filesystem::path project_root_path_;
  boost::filesystem::path project_src_path_;
  boost::filesystem::path project_bin_path_;
  std::vector<File> files_;
  boost::filesystem::path output_executable_path_;

  Logger logger_;

  void add_file(File file, boost::filesystem::path destination);
  boost::filesystem::path get_destination_path(FileType file_type);
  void create_directory(const boost::filesystem::path &path);
};
}
