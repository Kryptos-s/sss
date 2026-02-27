#pragma once

#include <optional>
#include <string>

namespace app {

enum class Screen {
  Login,
  Dashboard,
  Downloads,
  Settings
};

struct Config {
  std::string backendUrl;
  std::string keyAuthAppName;
  std::string keyAuthOwnerId;
  std::string keyAuthAppSecret;
  std::string keyAuthAppVersion;
  std::string appVersion;
  bool debugLogging{false};
};

struct Session {
  std::string token;
  std::string username;
  std::string userId;
};

struct BuildState {
  std::string buildId;
  std::string state;
  std::string downloadUrl;
  std::string sha256;
  std::string cooldownUntil;
  float progress{0.0F};
  std::string speed;
  std::string eta;
  std::string unzipPath;
};

struct AppState {
  Screen currentScreen{Screen::Login};
  Config config;
  std::optional<Session> session;
  std::string licenseInput;
  std::string statusMessage;
  std::string hwidHash;
  std::string hwidSource;
  std::string cooldownUntil;
  std::optional<BuildState> build;
};

}  // namespace app
