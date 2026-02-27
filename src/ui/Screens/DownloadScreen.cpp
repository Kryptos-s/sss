#include "ui/Screens/DownloadScreen.hpp"

#include <imgui.h>

#include "ui/Widgets/CopyButton.hpp"

namespace ui {
bool render_download_screen(app::AppState& state) {
  ImGui::Text("Downloads");
  bool request = ImGui::Button("Request build");
  if (state.build.has_value()) {
    ImGui::Text("Build ID: %s", state.build->buildId.c_str());
    ImGui::Text("State: %s", state.build->state.c_str());
    ImGui::ProgressBar(state.build->progress, {-1, 0});
    ImGui::Text("Speed: %s ETA: %s", state.build->speed.c_str(), state.build->eta.c_str());
    if (!state.build->unzipPath.empty()) {
      ImGui::TextWrapped("Load unpacked path: %s", state.build->unzipPath.c_str());
      copy_button("Copy load unpacked path", state.build->unzipPath);
    }
  }
  ImGui::BulletText("Chrome: open chrome://extensions, enable Developer mode, click Load unpacked, select extracted folder.");
  return request;
}
}
