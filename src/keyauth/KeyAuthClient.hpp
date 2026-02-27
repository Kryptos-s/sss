#pragma once

#include <string>

#include "keyauth/KeyAuthModels.hpp"
#include "net/HttpClient.hpp"

namespace keyauth {

class KeyAuthClient {
 public:
  KeyAuthClient(std::string appName, std::string ownerId, std::string appSecret, std::string version,
                std::string userAgent);

  bool init();
  UserInfo license_login(const std::string& licenseKey, const std::string& hwidSource);
  UserInfo fetch_user(const std::string& sessionToken);

 private:
  net::HttpClient http_;
  std::string appName_;
  std::string ownerId_;
  std::string appSecret_;
  std::string version_;
  std::string sessionId_;
};

}  // namespace keyauth
