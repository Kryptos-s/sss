#pragma once

#include <memory>

#include "app/State.hpp"
#include "keyauth/KeyAuthClient.hpp"
#include "net/ApiClient.hpp"
#include "security/SecureStore.hpp"

struct GLFWwindow;

namespace app {

class App {
 public:
  App();
  ~App();

  bool initialize();
  void run();

 private:
  void renderFrame();
  void renderSidebar();
  void routeScreens();
  void loadConfig();
  void saveConfig() const;

  GLFWwindow* window_{nullptr};
  AppState state_;
  std::unique_ptr<keyauth::KeyAuthClient> keyAuthClient_;
  std::unique_ptr<net::ApiClient> apiClient_;
  std::unique_ptr<security::SecureStore> secureStore_;
};

}  // namespace app
