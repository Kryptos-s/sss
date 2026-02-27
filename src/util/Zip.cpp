#include "util/Zip.hpp"

#include <filesystem>

#include <mz.h>
#include <mz_os.h>
#include <mz_zip.h>
#include <mz_zip_rw.h>

namespace fs = std::filesystem;

namespace util {

bool unzip_archive(const std::string& zipPath, const std::string& outDir, std::string& error) {
  fs::create_directories(outDir);
  void* reader = nullptr;
  if (mz_zip_reader_create(&reader) != MZ_OK) {
    error = "Failed to create zip reader";
    return false;
  }
  if (mz_zip_reader_open_file(reader, zipPath.c_str()) != MZ_OK) {
    mz_zip_reader_delete(&reader);
    error = "Failed to open zip";
    return false;
  }
  if (mz_zip_reader_save_all(reader, outDir.c_str()) != MZ_OK) {
    mz_zip_reader_close(reader);
    mz_zip_reader_delete(&reader);
    error = "Failed to extract zip";
    return false;
  }
  mz_zip_reader_close(reader);
  mz_zip_reader_delete(&reader);
  return true;
}

}  // namespace util
