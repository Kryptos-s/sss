#include "util/Time.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace util {

std::string now_iso8601() {
  const auto now = std::chrono::system_clock::now();
  const std::time_t tt = std::chrono::system_clock::to_time_t(now);
  std::tm tm{};
#ifdef _WIN32
  gmtime_s(&tm, &tt);
#else
  gmtime_r(&tt, &tm);
#endif
  std::ostringstream ss;
  ss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
  return ss.str();
}

long long seconds_until(const std::string& iso8601) {
  std::tm tm{};
  std::istringstream ss(iso8601);
  ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
  if (ss.fail()) return 0;
#ifdef _WIN32
  const time_t target = _mkgmtime(&tm);
#else
  const time_t target = timegm(&tm);
#endif
  return static_cast<long long>(target - std::time(nullptr));
}

}  // namespace util
