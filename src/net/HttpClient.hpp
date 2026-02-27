#pragma once

 #include <functional>
#include <map>
#include <optional>
#include <string>

#include <nlohmann/json.hpp>

namespace net {

struct HttpResponse {
  int status{0};
  std::string body;
  std::string error;
};

class HttpClient {
 public:
  explicit HttpClient(std::string userAgent);

  HttpResponse get_json(const std::string& url, const std::map<std::string, std::string>& headers) const;
  HttpResponse post_json(const std::string& url, const nlohmann::json& body,
                         const std::map<std::string, std::string>& headers) const;
  HttpResponse download_file(const std::string& url, const std::string& outPath,
                             std::function<void(std::int64_t, std::int64_t, double)> progressCb) const;

 private:
  HttpResponse request_with_retry(std::function<HttpResponse()> fn) const;
  std::string userAgent_;
};

}  // namespace net
