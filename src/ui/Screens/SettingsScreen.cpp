#include "ui/Screens/SettingsScreen.hpp"

#include <cstring>

#include <imgui.h>

namespace ui {
bool render_settings_screen(app::AppState& state) {
  ImGui::Text("Settings");
  static char backend[512] = "";
  static char appName[128] = "";
  static char owner[128] = "";
  static char secret[256] = "";
  static char kver[64] = "";
  static char aver[64] = "";
  strncpy(backend, state.config.backendUrl.c_str(), sizeof(backend) - 1);
  strncpy(appName, state.config.keyAuthAppName.c_str(), sizeof(appName) - 1);
  strncpy(owner, state.config.keyAuthOwnerId.c_str(), sizeof(owner) - 1);
  strncpy(secret, state.config.keyAuthAppSecret.c_str(), sizeof(secret) - 1);
  strncpy(kver, state.config.keyAuthAppVersion.c_str(), sizeof(kver) - 1);
  strncpy(aver, state.config.appVersion.c_str(), sizeof(aver) - 1);
  ImGui::InputText("Backend URL", backend, sizeof(backend));
  ImGui::InputText("KeyAuth App Name", appName, sizeof(appName));
  ImGui::InputText("KeyAuth Owner ID", owner, sizeof(owner));
  ImGui::InputText("KeyAuth App Secret", secret, sizeof(secret), ImGuiInputTextFlags_Password);
  ImGui::InputText("KeyAuth App Version", kver, sizeof(kver));
  ImGui::InputText("App Version", aver, sizeof(aver));
  ImGui::Checkbox("Debug logging", &state.config.debugLogging);
  if (ImGui::Button("Save Settings")) {
    state.config.backendUrl = backend;
    state.config.keyAuthAppName = appName;
    state.config.keyAuthOwnerId = owner;
    state.config.keyAuthAppSecret = secret;
    state.config.keyAuthAppVersion = kver;
    state.config.appVersion = aver;
    return true;
  }
  return false;
}
}
