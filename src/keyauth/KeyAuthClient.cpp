#include "keyauth/KeyAuthClient.hpp"

#include <nlohmann/json.hpp>

namespace {
constexpr const char* kKeyAuthBase = "https://keyauth.win/api/1.2/";
}

namespace keyauth {

KeyAuthClient::KeyAuthClient(std::string appName, std::string ownerId, std::string appSecret, std::string version,
                             std::string userAgent)
    : http_(std::move(userAgent)),
      appName_(std::move(appName)),
      ownerId_(std::move(ownerId)),
      appSecret_(std::move(appSecret)),
      version_(std::move(version)) {}

bool KeyAuthClient::init() {
  nlohmann::json body{{"type", "init"}, {"name", appName_}, {"ownerid", ownerId_}, {"ver", version_}, {"secret", appSecret_}};
  const auto r = http_.post_json(kKeyAuthBase, body, {});
  if (!r.error.empty()) return false;
  auto j = nlohmann::json::parse(r.body, nullptr, false);
  if (j.is_discarded() || !j.value("success", false)) return false;
  sessionId_ = j.value("sessionid", "");
  return !sessionId_.empty();
}

UserInfo KeyAuthClient::license_login(const std::string& licenseKey, const std::string& hwidSource) {
  nlohmann::json body{{"type", "license"},
                      {"name", appName_},
                      {"ownerid", ownerId_},
                      {"sessionid", sessionId_},
                      {"key", licenseKey},
                      {"hwid", hwidSource}};
  const auto r = http_.post_json(kKeyAuthBase, body, {});
  if (!r.error.empty()) return {.message = "NETWORK"};
  auto j = nlohmann::json::parse(r.body, nullptr, false);
  if (j.is_discarded()) return {.message = "SERVER_ERROR"};
  auto info = j.value("info", nlohmann::json::object());
  return {j.value("success", false),
          j.value("sessionid", ""),
          info.value("username", ""),
          info.value("uid", ""),
          info.value("subscription", ""),
          info.value("expiry", ""),
          j.value("message", "")};
}

UserInfo KeyAuthClient::fetch_user(const std::string& sessionToken) {
  nlohmann::json body{{"type", "fetchOnline"}, {"name", appName_}, {"ownerid", ownerId_}, {"sessionid", sessionToken}};
  const auto r = http_.post_json(kKeyAuthBase, body, {});
  if (!r.error.empty()) return {.message = "NETWORK"};
  auto j = nlohmann::json::parse(r.body, nullptr, false);
  if (j.is_discarded()) return {.message = "SERVER_ERROR"};
  return {j.value("success", false), sessionToken, "", "", "", "", j.value("message", "")};
}

}  // namespace keyauth
