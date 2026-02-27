#pragma once
#include <string>

namespace util {
bool unzip_archive(const std::string& zipPath, const std::string& outDir, std::string& error);
}
