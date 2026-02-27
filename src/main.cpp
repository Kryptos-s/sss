#include "app/App.hpp"

int main() {
  app::App app;
  if (!app.initialize()) {
    return 1;
  }
  app.run();
  return 0;
}
