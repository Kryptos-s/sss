#pragma once

#include <string>
#include <vector>

namespace security {

std::string sha256_hex(const std::string& input);
std::string sha256_file_hex(const std::string& path);
std::vector<unsigned char> secure_random_bytes(std::size_t count);

}  // namespace security
