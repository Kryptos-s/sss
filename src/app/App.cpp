#include "app/App.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <nlohmann/json.hpp>

#include "security/Crypto.hpp"
#include "security/Hwid.hpp"
#include "ui/Screens/DashboardScreen.hpp"
#include "ui/Screens/DownloadScreen.hpp"
#include "ui/Screens/LoginScreen.hpp"
#include "ui/Screens/SettingsScreen.hpp"
#include "ui/Theme.hpp"
#include "ui/Widgets/Toasts.hpp"
#include "util/Files.hpp"
#include "util/Log.hpp"
#include "util/Time.hpp"
#include "util/Zip.hpp"

namespace fs = std::filesystem;

namespace app {

App::App() = default;
App::~App() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  if (window_ != nullptr) glfwDestroyWindow(window_);
  glfwTerminate();
}

void App::loadConfig() {
  state_.config.backendUrl = std::getenv("ACTIVATOR_BACKEND_URL") ? std::getenv("ACTIVATOR_BACKEND_URL") : "";
  state_.config.keyAuthAppName = std::getenv("KEYAUTH_APP_NAME") ? std::getenv("KEYAUTH_APP_NAME") : "Activator";
  state_.config.keyAuthOwnerId = std::getenv("KEYAUTH_OWNER_ID") ? std::getenv("KEYAUTH_OWNER_ID") : "";
  state_.config.keyAuthAppSecret = std::getenv("KEYAUTH_APP_SECRET") ? std::getenv("KEYAUTH_APP_SECRET") : "";
  state_.config.keyAuthAppVersion = std::getenv("KEYAUTH_APP_VERSION") ? std::getenv("KEYAUTH_APP_VERSION") : "1.0";
  state_.config.appVersion = std::getenv("APP_VERSION") ? std::getenv("APP_VERSION") : "1.0.0";

  const auto cfgPath = util::appdata_dir(state_.config.keyAuthAppName) + "/config.json";
  std::ifstream in(cfgPath);
  if (in) {
    auto j = nlohmann::json::parse(in, nullptr, false);
    if (!j.is_discarded()) {
      state_.config.backendUrl = j.value("backendUrl", state_.config.backendUrl);
      state_.config.keyAuthAppName = j.value("keyAuthAppName", state_.config.keyAuthAppName);
      state_.config.keyAuthOwnerId = j.value("keyAuthOwnerId", state_.config.keyAuthOwnerId);
      state_.config.keyAuthAppSecret = j.value("keyAuthAppSecret", state_.config.keyAuthAppSecret);
      state_.config.keyAuthAppVersion = j.value("keyAuthAppVersion", state_.config.keyAuthAppVersion);
      state_.config.appVersion = j.value("appVersion", state_.config.appVersion);
      state_.config.debugLogging = j.value("debugLogging", false);
    }
  }
}

void App::saveConfig() const {
  const auto dir = util::appdata_dir(state_.config.keyAuthAppName);
  util::ensure_dir(dir);
  nlohmann::json j{{"backendUrl", state_.config.backendUrl},
                   {"keyAuthAppName", state_.config.keyAuthAppName},
                   {"keyAuthOwnerId", state_.config.keyAuthOwnerId},
                   {"keyAuthAppSecret", state_.config.keyAuthAppSecret},
                   {"keyAuthAppVersion", state_.config.keyAuthAppVersion},
                   {"appVersion", state_.config.appVersion},
                   {"debugLogging", state_.config.debugLogging}};
  std::ofstream(dir + "/config.json") << j.dump(2);
}

bool App::initialize() {
  loadConfig();
  util::init_logging(state_.config.keyAuthAppName, state_.config.debugLogging);
  state_.hwidSource = security::build_hwid_source();
  state_.hwidHash = security::hwid_hash(state_.hwidSource);

  secureStore_ = std::make_unique<security::SecureStore>(state_.config.keyAuthAppName);
  state_.session = secureStore_->load_session();
  if (state_.session.has_value()) state_.currentScreen = Screen::Dashboard;

  keyAuthClient_ = std::make_unique<keyauth::KeyAuthClient>(state_.config.keyAuthAppName, state_.config.keyAuthOwnerId,
                                                            state_.config.keyAuthAppSecret, state_.config.keyAuthAppVersion,
                                                            "Activator/" + state_.config.appVersion);
  apiClient_ = std::make_unique<net::ApiClient>(state_.config.backendUrl, "Activator/" + state_.config.appVersion);

  if (!glfwInit()) return false;
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  window_ = glfwCreateWindow(1100, 720, "Activator", nullptr, nullptr);
  if (window_ == nullptr) return false;
  glfwMakeContextCurrent(window_);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return false;

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(window_, true);
  ImGui_ImplOpenGL3_Init("#version 330");
  ui::apply_theme();
  return true;
}

void App::renderSidebar() {
  ImGui::Begin("Navigation", nullptr, ImGuiWindowFlags_NoResize);
  if (!state_.session.has_value()) {
    if (ImGui::Selectable("Login", state_.currentScreen == Screen::Login)) state_.currentScreen = Screen::Login;
  }
  if (ImGui::Selectable("Dashboard", state_.currentScreen == Screen::Dashboard)) state_.currentScreen = Screen::Dashboard;
  if (ImGui::Selectable("Downloads", state_.currentScreen == Screen::Downloads)) state_.currentScreen = Screen::Downloads;
  if (ImGui::Selectable("Settings", state_.currentScreen == Screen::Settings)) state_.currentScreen = Screen::Settings;

  if (state_.session.has_value() && ImGui::Button("Logout")) {
    secureStore_->clear_session();
    state_.session.reset();
    state_.build.reset();
    state_.currentScreen = Screen::Login;
  }
  ImGui::End();
}

void App::routeScreens() {
  ImGui::Begin("Content");
  if (state_.currentScreen == Screen::Login) {
    if (ui::render_login_screen(state_)) {
      if (keyAuthClient_->init()) {
        const auto info = keyAuthClient_->license_login(state_.licenseInput, state_.hwidSource);
        if (info.success) {
          state_.session = Session{info.token, info.username, info.userId};
          secureStore_->save_session(*state_.session);
          auto ent = apiClient_->verify_entitlements(state_.session->token, state_.hwidHash, state_.config.appVersion);
          state_.cooldownUntil = ent.cooldownUntil;
          state_.currentScreen = Screen::Dashboard;
          ui::add_toast("Login successful");
        } else {
          ui::add_toast("Login failed: " + info.message, true);
        }
      } else {
        ui::add_toast("KeyAuth init failed", true);
      }
    }
  } else if (state_.currentScreen == Screen::Dashboard) {
    ui::render_dashboard_screen(state_);
  } else if (state_.currentScreen == Screen::Downloads) {
    if (ui::render_download_screen(state_)) {
      const auto remain = util::seconds_until(state_.cooldownUntil);
      if (remain > 0) {
        ui::add_toast("Cooldown active", true);
      } else if (state_.session.has_value()) {
        auto build = apiClient_->request_build(state_.session->token, state_.hwidHash, "stable", "latest");
        state_.cooldownUntil = build.cooldownUntil;
        state_.build = BuildState{build.buildId, build.state, build.downloadUrl, build.sha256, build.cooldownUntil};
        if (build.state == "ready" && !build.downloadUrl.empty()) {
          const auto dlDir = util::appdata_dir(state_.config.keyAuthAppName) + "/downloads";
          util::ensure_dir(dlDir);
          const auto zipPath = dlDir + "/" + build.buildId + ".zip";
          auto dr = apiClient_->download(build.downloadUrl, zipPath, [&](std::int64_t now, std::int64_t total, double) {
            if (state_.build.has_value() && total > 0) state_.build->progress = static_cast<float>(now) / static_cast<float>(total);
          });
          if (dr.error.empty()) {
            const auto hash = security::sha256_file_hex(zipPath);
            if (hash == build.sha256) {
              std::string err;
              const auto out = dlDir + "/" + build.buildId;
              if (util::unzip_archive(zipPath, out, err)) {
                state_.build->unzipPath = out;
                ui::add_toast("Build downloaded and verified");
              } else {
                ui::add_toast("Unzip failed: " + err, true);
              }
            } else {
              ui::add_toast("SHA-256 mismatch", true);
            }
          } else {
            ui::add_toast("Download failed", true);
          }
        }
      }
    }
  } else if (state_.currentScreen == Screen::Settings) {
    if (ui::render_settings_screen(state_)) {
      saveConfig();
      ui::add_toast("Settings saved");
    }
  }

  if (ImGui::Button("Copy diagnostics")) {
    std::string d = "app=" + state_.config.appVersion + " backend=" + state_.config.backendUrl + " keyauth=" + state_.config.keyAuthAppName + "\n";
    for (const auto& line : util::tail_logs(50)) d += line + "\n";
    ImGui::SetClipboardText(d.c_str());
    ui::add_toast("Diagnostics copied");
  }
  ImGui::End();
}

void App::renderFrame() {
  glfwPollEvents();
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  renderSidebar();
  routeScreens();
  ui::render_toasts();

  ImGui::Render();
  int w = 0, h = 0;
  glfwGetFramebufferSize(window_, &w, &h);
  glViewport(0, 0, w, h);
  glClearColor(0.08F, 0.08F, 0.1F, 1.0F);
  glClear(GL_COLOR_BUFFER_BIT);
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  glfwSwapBuffers(window_);
}

void App::run() {
  while (!glfwWindowShouldClose(window_)) {
    renderFrame();
  }
}

}  // namespace app
