#include "util/Log.hpp"

#include <deque>
#include <filesystem>
#include <fstream>
#include <mutex>

#include "util/Files.hpp"
#include "util/Strings.hpp"
#include "util/Time.hpp"

namespace {
std::mutex g_mu;
std::string g_file;
bool g_debug{false};

void write(const char* level, const std::string& msg) {
  std::lock_guard<std::mutex> lock(g_mu);
  std::ofstream out(g_file, std::ios::app);
  out << util::now_iso8601() << " [" << level << "] " << util::sanitize_secret(msg) << "\n";
}
}

namespace util {

void init_logging(const std::string& appName, bool debug) {
  g_debug = debug;
  const auto base = appdata_dir(appName) + "/logs";
  ensure_dir(base);
  g_file = base + "/app.log";
}

void log_info(const std::string& msg) { write("info", msg); }
void log_warn(const std::string& msg) { write("warn", msg); }
void log_error(const std::string& msg) { write("error", msg); }

std::vector<std::string> tail_logs(std::size_t maxLines) {
  std::lock_guard<std::mutex> lock(g_mu);
  std::ifstream file(g_file);
  std::deque<std::string> q;
  std::string line;
  while (std::getline(file, line)) {
    q.push_back(line);
    if (q.size() > maxLines) q.pop_front();
  }
  return {q.begin(), q.end()};
}

}  // namespace util
