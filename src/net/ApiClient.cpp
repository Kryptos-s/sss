#include "net/ApiClient.hpp"

namespace {
std::map<std::string, std::string> auth_headers(const std::string& bearer) {
  return {{"Authorization", "Bearer " + bearer}};
}
}

namespace net {

ApiClient::ApiClient(std::string baseUrl, std::string userAgent)
    : http_(std::move(userAgent)), baseUrl_(std::move(baseUrl)) {}

EntitlementResponse ApiClient::verify_entitlements(const std::string& bearer, const std::string& hwidHash,
                                                   const std::string& appVersion) const {
  const auto r = http_.post_json(baseUrl_ + "/v1/entitlements/verify",
                                 {{"hwidHash", hwidHash}, {"appVersion", appVersion}}, auth_headers(bearer));
  if (!r.error.empty()) return {.error = "NETWORK"};
  auto j = nlohmann::json::parse(r.body, nullptr, false);
  if (j.is_discarded()) return {.error = "SERVER_ERROR"};
  return {j.value("ok", false), j.value("userId", ""), j.value("plan", ""), j.value("expiresAt", ""),
          j.value("cooldownUntil", ""), j.value("bound", false), j.value("boundHwidHint", ""), j.value("error", "")};
}

BuildResponse ApiClient::request_build(const std::string& bearer, const std::string& hwidHash,
                                       const std::string& channel, const std::string& extensionVersion) const {
  const auto r = http_.post_json(baseUrl_ + "/v1/build/request",
                                 {{"hwidHash", hwidHash}, {"channel", channel}, {"extensionVersion", extensionVersion}},
                                 auth_headers(bearer));
  if (!r.error.empty()) return {.error = "NETWORK"};
  auto j = nlohmann::json::parse(r.body, nullptr, false);
  if (j.is_discarded()) return {.error = "SERVER_ERROR"};
  return {j.value("ok", false), j.value("buildId", ""), j.value("state", ""), j.value("downloadUrl", ""),
          j.value("sha256", ""), j.value("cooldownUntil", ""), j.value("message", ""), j.value("error", "")};
}

BuildResponse ApiClient::build_status(const std::string& buildId) const {
  const auto r = http_.get_json(baseUrl_ + "/v1/build/status?buildId=" + buildId, {});
  if (!r.error.empty()) return {.error = "NETWORK"};
  auto j = nlohmann::json::parse(r.body, nullptr, false);
  if (j.is_discarded()) return {.error = "SERVER_ERROR"};
  return {j.value("ok", false), j.value("buildId", buildId), j.value("state", ""), j.value("downloadUrl", ""),
          j.value("sha256", ""), j.value("cooldownUntil", ""), j.value("message", ""), j.value("error", "")};
}

HttpResponse ApiClient::download(const std::string& url, const std::string& outPath,
                                 std::function<void(std::int64_t, std::int64_t, double)> progressCb) const {
  return http_.download_file(url, outPath, std::move(progressCb));
}

}  // namespace net
