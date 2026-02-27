#pragma once

#include <string>

namespace security {

std::string build_hwid_source();
std::string hwid_hash(const std::string& source);
std::string short_hash(const std::string& hash);

}  // namespace security
