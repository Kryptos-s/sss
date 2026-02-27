#pragma once

#include <string>

namespace util {

std::string appdata_dir(const std::string& appName);
bool ensure_dir(const std::string& path);
bool remove_file(const std::string& path);
bool open_folder(const std::string& path);

}  // namespace util
