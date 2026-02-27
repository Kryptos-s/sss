#include <cstdlib>
#include <iostream>

#include "security/Crypto.hpp"
#include "security/Hwid.hpp"

int main() {
  const auto digest = security::sha256_hex("abc");
  if (digest != "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad") {
    std::cerr << "sha256 failed\n";
    return EXIT_FAILURE;
  }
  const auto shorty = security::short_hash("1234567890");
  if (shorty != "12345678") {
    std::cerr << "short hash failed\n";
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
