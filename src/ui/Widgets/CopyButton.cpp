#include "ui/Widgets/CopyButton.hpp"
#include <imgui.h>
namespace ui {
bool copy_button(const char* label, const std::string& text) {
  if (ImGui::Button(label)) { ImGui::SetClipboardText(text.c_str()); return true; }
  return false;
}
}
