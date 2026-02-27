#include "security/SecureStore.hpp"

#include <filesystem>
#include <fstream>

#include <nlohmann/json.hpp>

#include "app/State.hpp"
#include "util/Files.hpp"

#ifdef _WIN32
#include <windows.h>
#include <dpapi.h>
#endif

namespace fs = std::filesystem;

namespace security {

SecureStore::SecureStore(std::string appName) : appName_(std::move(appName)) {}

std::string SecureStore::sessionPath() const {
  return util::appdata_dir(appName_) + "/session.dat";
}

bool SecureStore::save_session(const app::Session& session) const {
#ifdef _WIN32
  nlohmann::json data{{"token", session.token}, {"username", session.username}, {"userId", session.userId}};
  const auto plain = data.dump();
  DATA_BLOB in{static_cast<DWORD>(plain.size()), reinterpret_cast<BYTE*>(const_cast<char*>(plain.data()))};
  DATA_BLOB out{};
  if (!CryptProtectData(&in, L"ActivatorSession", nullptr, nullptr, nullptr, CRYPTPROTECT_LOCAL_MACHINE, &out)) {
    return false;
  }
  fs::create_directories(fs::path(sessionPath()).parent_path());
  std::ofstream file(sessionPath(), std::ios::binary);
  file.write(reinterpret_cast<const char*>(out.pbData), static_cast<std::streamsize>(out.cbData));
  LocalFree(out.pbData);
  return static_cast<bool>(file);
#else
  (void)session;
  return false;
#endif
}

std::optional<app::Session> SecureStore::load_session() const {
#ifdef _WIN32
  std::ifstream file(sessionPath(), std::ios::binary);
  if (!file) return std::nullopt;
  const std::string blob((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  DATA_BLOB in{static_cast<DWORD>(blob.size()), reinterpret_cast<BYTE*>(const_cast<char*>(blob.data()))};
  DATA_BLOB out{};
  if (!CryptUnprotectData(&in, nullptr, nullptr, nullptr, nullptr, 0, &out)) return std::nullopt;
  const std::string plain(reinterpret_cast<const char*>(out.pbData), out.cbData);
  LocalFree(out.pbData);
  const auto j = nlohmann::json::parse(plain, nullptr, false);
  if (j.is_discarded()) return std::nullopt;
  return app::Session{j.value("token", ""), j.value("username", ""), j.value("userId", "")};
#else
  return std::nullopt;
#endif
}

bool SecureStore::clear_session() const { return util::remove_file(sessionPath()); }

}  // namespace security
