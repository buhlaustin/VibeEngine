#pragma once

#include "vibe/project/Project.hpp"

#include <memory>
#include <string>

namespace vibe {

class Platform;

struct RuntimeConfig {
  std::string projectRoot;
  Project project;
};

class Application {
 public:
  explicit Application(RuntimeConfig config);
  ~Application();

  Application(const Application&) = delete;
  Application& operator=(const Application&) = delete;

  int run();

 private:
  RuntimeConfig config_;
  std::unique_ptr<Platform> platform_;
  bool running_{true};
};

}  // namespace vibe
