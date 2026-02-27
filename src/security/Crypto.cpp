#include "security/Crypto.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

#include <openssl/evp.h>
#include <openssl/rand.h>

namespace {
std::string to_hex(const unsigned char* data, std::size_t len) {
  static constexpr char kHex[] = "0123456789abcdef";
  std::string out;
  out.resize(len * 2);
  for (std::size_t i = 0; i < len; ++i) {
    out[i * 2] = kHex[(data[i] >> 4U) & 0x0FU];
    out[i * 2 + 1] = kHex[data[i] & 0x0FU];
  }
  return out;
}
}

namespace security {

std::string sha256_hex(const std::string& input) {
  EVP_MD_CTX* ctx = EVP_MD_CTX_new();
  if (ctx == nullptr) throw std::runtime_error("EVP_MD_CTX_new failed");

  unsigned char hash[EVP_MAX_MD_SIZE];
  unsigned int len = 0;

  if (EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr) != 1 ||
      EVP_DigestUpdate(ctx, input.data(), input.size()) != 1 ||
      EVP_DigestFinal_ex(ctx, hash, &len) != 1) {
    EVP_MD_CTX_free(ctx);
    throw std::runtime_error("SHA-256 failed");
  }

  EVP_MD_CTX_free(ctx);
  return to_hex(hash, len);
}

std::string sha256_file_hex(const std::string& path) {
  std::ifstream file(path, std::ios::binary);
  if (!file) throw std::runtime_error("Cannot open file for hashing");

  EVP_MD_CTX* ctx = EVP_MD_CTX_new();
  if (ctx == nullptr) throw std::runtime_error("EVP_MD_CTX_new failed");
  if (EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr) != 1) {
    EVP_MD_CTX_free(ctx);
    throw std::runtime_error("SHA init failed");
  }

  char buffer[4096];
  while (file.good()) {
    file.read(buffer, sizeof(buffer));
    const auto got = static_cast<std::size_t>(file.gcount());
    if (got > 0 && EVP_DigestUpdate(ctx, buffer, got) != 1) {
      EVP_MD_CTX_free(ctx);
      throw std::runtime_error("SHA update failed");
    }
  }

  unsigned char hash[EVP_MAX_MD_SIZE];
  unsigned int len = 0;
  if (EVP_DigestFinal_ex(ctx, hash, &len) != 1) {
    EVP_MD_CTX_free(ctx);
    throw std::runtime_error("SHA final failed");
  }
  EVP_MD_CTX_free(ctx);
  return to_hex(hash, len);
}

std::vector<unsigned char> secure_random_bytes(std::size_t count) {
  std::vector<unsigned char> out(count);
  if (RAND_bytes(out.data(), static_cast<int>(count)) != 1) {
    throw std::runtime_error("RAND_bytes failed");
  }
  return out;
}

}  // namespace security
