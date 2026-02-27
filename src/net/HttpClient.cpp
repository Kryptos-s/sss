#include "net/HttpClient.hpp"

#include <thread>

#include <cpr/cpr.h>

namespace net {

HttpClient::HttpClient(std::string userAgent) : userAgent_(std::move(userAgent)) {}

HttpResponse HttpClient::request_with_retry(std::function<HttpResponse()> fn) const {
  for (int i = 0; i < 3; ++i) {
    auto res = fn();
    if (res.error.empty() && !(res.status == 502 || res.status == 503)) return res;
    std::this_thread::sleep_for(std::chrono::milliseconds(250 * (1 << i)));
  }
  return fn();
}

HttpResponse HttpClient::get_json(const std::string& url, const std::map<std::string, std::string>& headers) const {
  return request_with_retry([&]() {
    cpr::Header h(headers.begin(), headers.end());
    h["User-Agent"] = userAgent_;
    const auto r = cpr::Get(cpr::Url{url}, h, cpr::ConnectTimeout{5000}, cpr::Timeout{15000}, cpr::VerifySsl{true});
    return HttpResponse{r.status_code, r.text, r.error.message};
  });
}

HttpResponse HttpClient::post_json(const std::string& url, const nlohmann::json& body,
                                   const std::map<std::string, std::string>& headers) const {
  return request_with_retry([&]() {
    cpr::Header h(headers.begin(), headers.end());
    h["Content-Type"] = "application/json";
    h["User-Agent"] = userAgent_;
    const auto r = cpr::Post(cpr::Url{url}, h, cpr::Body{body.dump()}, cpr::ConnectTimeout{5000},
                             cpr::Timeout{15000}, cpr::VerifySsl{true});
    return HttpResponse{r.status_code, r.text, r.error.message};
  });
}

HttpResponse HttpClient::download_file(const std::string& url, const std::string& outPath,
                                       std::function<void(std::int64_t, std::int64_t, double)> progressCb) const {
  return request_with_retry([&]() {
    cpr::Session s;
    s.SetUrl(cpr::Url{url});
    s.SetConnectTimeout(cpr::ConnectTimeout{5000});
    s.SetTimeout(cpr::Timeout{15000});
    s.SetVerifySsl(cpr::VerifySsl{true});
    s.SetUserAgent(userAgent_);
    s.SetProgressCallback([&](cpr::cpr_off_t dltotal, cpr::cpr_off_t dlnow, cpr::cpr_off_t, cpr::cpr_off_t, cpr::cpr_off_t) {
      progressCb(dlnow, dltotal, 0.0);
      return true;
    });
    const auto r = s.Download(outPath);
    return HttpResponse{r.status_code, "", r.error.message};
  });
}

}  // namespace net
