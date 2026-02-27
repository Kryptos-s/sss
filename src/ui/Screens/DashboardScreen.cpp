#include "ui/Screens/DashboardScreen.hpp"

#include <imgui.h>

#include "security/Hwid.hpp"
#include "ui/Widgets/CopyButton.hpp"

namespace ui {
void render_dashboard_screen(app::AppState& state) {
  ImGui::Text("Dashboard");
  ImGui::Separator();
  ImGui::Text("HWID Hash: %s", security::short_hash(state.hwidHash).c_str());
  ImGui::SameLine();
  copy_button("Copy full HWID hash", state.hwidHash);
  if (state.session.has_value()) {
    ImGui::Text("User: %s", state.session->username.c_str());
    ImGui::Text("User ID: %s", state.session->userId.c_str());
  }
  ImGui::Text("Cooldown until: %s", state.cooldownUntil.empty() ? "n/a" : state.cooldownUntil.c_str());
}
}
