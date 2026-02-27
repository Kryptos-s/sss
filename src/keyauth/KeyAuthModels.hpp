#pragma once

#include <string>

namespace keyauth {

struct UserInfo {
  bool success{false};
  std::string token;
  std::string username;
  std::string userId;
  std::string plan;
  std::string expiry;
  std::string message;
};

}  // namespace keyauth
