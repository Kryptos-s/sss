#include "util/Files.hpp"

#include <cstdlib>
#include <filesystem>

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#endif

namespace fs = std::filesystem;

namespace util {

std::string appdata_dir(const std::string& appName) {
#ifdef _WIN32
  char path[MAX_PATH];
  SHGetFolderPathA(nullptr, CSIDL_APPDATA, nullptr, 0, path);
  return (fs::path(path) / appName).string();
#else
  return (fs::path(std::getenv("HOME")) / ("." + appName)).string();
#endif
}

bool ensure_dir(const std::string& path) { return fs::create_directories(path) || fs::exists(path); }

bool remove_file(const std::string& path) { return !fs::exists(path) || fs::remove(path); }

bool open_folder(const std::string& path) {
#ifdef _WIN32
  const auto wide = std::wstring(path.begin(), path.end());
  return reinterpret_cast<intptr_t>(ShellExecuteW(nullptr, L"open", wide.c_str(), nullptr, nullptr, SW_SHOWDEFAULT)) > 32;
#else
  (void)path;
  return false;
#endif
}

}  // namespace util
