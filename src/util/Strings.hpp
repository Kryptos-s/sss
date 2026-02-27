#pragma once
#include <string>

namespace util {
std::string trim(const std::string& s);
std::string sanitize_secret(const std::string& s);
}
