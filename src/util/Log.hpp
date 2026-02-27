#pragma once
#include <string>
#include <vector>

namespace util {

void init_logging(const std::string& appName, bool debug);
void log_info(const std::string& msg);
void log_warn(const std::string& msg);
void log_error(const std::string& msg);
std::vector<std::string> tail_logs(std::size_t maxLines);

}  // namespace util
