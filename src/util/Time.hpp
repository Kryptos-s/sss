#pragma once
#include <string>

namespace util {

std::string now_iso8601();
long long seconds_until(const std::string& iso8601);

}  // namespace util
