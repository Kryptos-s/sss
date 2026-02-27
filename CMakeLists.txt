cmake_minimum_required(VERSION 3.21)
project(Activator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(imgui CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(cpr CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(unofficial-minizip CONFIG REQUIRED)

set(SOURCES
  src/main.cpp
  src/app/App.cpp
  src/ui/Theme.cpp
  src/ui/Screens/LoginScreen.cpp
  src/ui/Screens/DashboardScreen.cpp
  src/ui/Screens/DownloadScreen.cpp
  src/ui/Screens/SettingsScreen.cpp
  src/ui/Widgets/Toasts.cpp
  src/ui/Widgets/Spinner.cpp
  src/ui/Widgets/CopyButton.cpp
  src/keyauth/KeyAuthClient.cpp
  src/net/HttpClient.cpp
  src/net/ApiClient.cpp
  src/security/Hwid.cpp
  src/security/Crypto.cpp
  src/security/SecureStore.cpp
  src/util/Files.cpp
  src/util/Time.cpp
  src/util/Strings.cpp
  src/util/Zip.cpp
  src/util/Log.cpp
)

add_executable(Activator ${SOURCES})
target_include_directories(Activator PRIVATE src)

target_link_libraries(Activator PRIVATE
  imgui::imgui
  glfw
  glad::glad
  cpr::cpr
  OpenSSL::SSL
  OpenSSL::Crypto
  nlohmann_json::nlohmann_json
  fmt::fmt
  unofficial::minizip::minizip
  shlwapi
  crypt32
)

if (WIN32)
  target_compile_definitions(Activator PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
endif()

enable_testing()
add_executable(crypto_tests
  tests/crypto_tests.cpp
  src/security/Crypto.cpp
  src/security/Hwid.cpp
)
target_include_directories(crypto_tests PRIVATE src)
target_link_libraries(crypto_tests PRIVATE OpenSSL::Crypto)
if (WIN32)
  target_compile_definitions(crypto_tests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
endif()
add_test(NAME crypto_tests COMMAND crypto_tests)
