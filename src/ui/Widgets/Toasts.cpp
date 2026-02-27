#include "ui/Widgets/Toasts.hpp"

#include <algorithm>
#include <chrono>

#include <imgui.h>

namespace {
std::vector<ui::Toast> g_toasts;
double now_sec() {
  using namespace std::chrono;
  return duration<double>(steady_clock::now().time_since_epoch()).count();
}
}

namespace ui {
void add_toast(const std::string& msg, bool error) { g_toasts.push_back({msg, error, now_sec() + 4.0}); }
void render_toasts() {
  const auto n = now_sec();
  g_toasts.erase(std::remove_if(g_toasts.begin(), g_toasts.end(), [=](const Toast& t) { return n > t.expiresAt; }), g_toasts.end());
  ImGui::SetNextWindowBgAlpha(0.85F);
  ImGui::SetNextWindowPos({ImGui::GetIO().DisplaySize.x - 20.0F, 20.0F}, ImGuiCond_Always, {1.0F, 0.0F});
  if (ImGui::Begin("##toasts", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNav)) {
    for (const auto& t : g_toasts) {
      ImGui::TextColored(t.error ? ImVec4(1, 0.4F, 0.4F, 1) : ImVec4(0.4F, 1, 0.4F, 1), "%s", t.msg.c_str());
    }
  }
  ImGui::End();
}
}
