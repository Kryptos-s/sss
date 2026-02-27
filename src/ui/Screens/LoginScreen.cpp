#include "ui/Screens/LoginScreen.hpp"

#include <imgui.h>

namespace ui {
bool render_login_screen(app::AppState& state) {
  ImGui::Text("License Login");
  static char license[256] = "";
  ImGui::InputText("License", license, sizeof(license));
  state.licenseInput = license;
  return ImGui::Button("Login");
}
}
