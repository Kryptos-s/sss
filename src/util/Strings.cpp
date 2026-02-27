#include "util/Strings.hpp"

#include <regex>

namespace util {
std::string trim(const std::string& s) {
  const auto start = s.find_first_not_of(" \t\n\r");
  if (start == std::string::npos) return "";
  const auto end = s.find_last_not_of(" \t\n\r");
  return s.substr(start, end - start + 1);
}

std::string sanitize_secret(const std::string& s) {
  std::string out = std::regex_replace(s, std::regex("(?i)(authorization: Bearer )[^\\s]+"), "$1[REDACTED]");
  out = std::regex_replace(out, std::regex("(?i)(license|token|secret)=[^&\\s]+"), "$1=[REDACTED]");
  return out;
}
}  // namespace util
