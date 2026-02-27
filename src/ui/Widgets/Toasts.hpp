#pragma once

#include <string>
#include <vector>

namespace ui {
struct Toast { std::string msg; bool error{false}; double expiresAt{0}; };
void add_toast(const std::string& msg, bool error = false);
void render_toasts();
}
