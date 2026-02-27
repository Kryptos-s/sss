#pragma once

#include <optional>
#include <string>

namespace app { struct Session; }

namespace security {

class SecureStore {
 public:
  explicit SecureStore(std::string appName);

  bool save_session(const app::Session& session) const;
  std::optional<app::Session> load_session() const;
  bool clear_session() const;

 private:
  std::string sessionPath() const;
  std::string appName_;
};

}  // namespace security
