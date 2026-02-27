#include "ui/Theme.hpp"

#include <imgui.h>

namespace ui {
void apply_theme() {
  auto& s = ImGui::GetStyle();
  s.WindowRounding = 8.0F;
  s.FrameRounding = 6.0F;
  s.GrabRounding = 6.0F;
  s.ItemSpacing = {10.0F, 8.0F};
  ImGui::StyleColorsDark();
}
}
