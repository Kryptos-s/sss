#include "security/Hwid.hpp"

#include <cstring>
#include <sstream>

#include "security/Crypto.hpp"

#ifdef _WIN32
#include <windows.h>
#include <winreg.h>
#include <intrin.h>
#endif

namespace {
#ifdef _WIN32
std::string read_machine_guid() {
  HKEY key;
  if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Cryptography", 0, KEY_READ | KEY_WOW64_64KEY, &key) != ERROR_SUCCESS) {
    return "unknown-guid";
  }
  char buffer[256];
  DWORD type = REG_SZ;
  DWORD size = sizeof(buffer);
  const auto status = RegQueryValueExA(key, "MachineGuid", nullptr, &type, reinterpret_cast<LPBYTE>(buffer), &size);
  RegCloseKey(key);
  if (status != ERROR_SUCCESS) return "unknown-guid";
  return std::string(buffer);
}

std::string read_volume_serial() {
  DWORD serial = 0;
  if (!GetVolumeInformationA("C:\\", nullptr, 0, &serial, nullptr, nullptr, nullptr, 0)) {
    return "unknown-vol";
  }
  std::ostringstream ss;
  ss << std::hex << serial;
  return ss.str();
}

std::string read_cpu_brand() {
  int data[4] = {-1};
  char brand[0x40] = {0};
  __cpuid(data, 0x80000000);
  unsigned int max_ext = data[0];
  if (max_ext >= 0x80000004) {
    __cpuid(reinterpret_cast<int*>(data), 0x80000002);
    memcpy(brand, data, sizeof(data));
    __cpuid(reinterpret_cast<int*>(data), 0x80000003);
    memcpy(brand + 16, data, sizeof(data));
    __cpuid(reinterpret_cast<int*>(data), 0x80000004);
    memcpy(brand + 32, data, sizeof(data));
  }
  return std::string(brand);
}
#endif
}

namespace security {

std::string build_hwid_source() {
#ifdef _WIN32
  return read_machine_guid() + "|" + read_volume_serial() + "|" + read_cpu_brand();
#else
  return "unsupported-platform";
#endif
}

std::string hwid_hash(const std::string& source) { return sha256_hex(source); }

std::string short_hash(const std::string& hash) {
  return hash.substr(0, hash.size() >= 8 ? 8 : hash.size());
}

}  // namespace security
