#pragma once

#include <functional>
#include <optional>
#include <string>

#include <nlohmann/json.hpp>

#include "net/HttpClient.hpp"

namespace net {

struct EntitlementResponse {
  bool ok{false};
  std::string userId;
  std::string plan;
  std::string expiresAt;
  std::string cooldownUntil;
  bool bound{false};
  std::string boundHwidHint;
  std::string error;
};

struct BuildResponse {
  bool ok{false};
  std::string buildId;
  std::string state;
  std::string downloadUrl;
  std::string sha256;
  std::string cooldownUntil;
  std::string message;
  std::string error;
};

class ApiClient {
 public:
  ApiClient(std::string baseUrl, std::string userAgent);

  EntitlementResponse verify_entitlements(const std::string& bearer, const std::string& hwidHash,
                                          const std::string& appVersion) const;
  BuildResponse request_build(const std::string& bearer, const std::string& hwidHash,
                              const std::string& channel, const std::string& extensionVersion) const;
  BuildResponse build_status(const std::string& buildId) const;
  HttpResponse download(const std::string& url, const std::string& outPath,
                        std::function<void(std::int64_t, std::int64_t, double)> progressCb) const;

 private:
  HttpClient http_;
  std::string baseUrl_;
};

}  // namespace net
