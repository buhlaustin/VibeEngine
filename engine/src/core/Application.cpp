#include "vibe/core/Application.hpp"

#include "vibe/core/Log.hpp"
#include "vibe/platform/Platform.hpp"
#include "vibe/scene/Scene.hpp"

#include <filesystem>

namespace vibe {

Application::Application(RuntimeConfig config) : config_(std::move(config)) {
  platform_ = createPlatform();
}

Application::~Application() {
  if (platform_) {
    platform_->shutdown();
  }
}

int Application::run() {
  PlatformInit init{};
  init.windowTitle = config_.project.name.c_str();
  init.width = 1280;
  init.height = 720;

  if (!platform_->initialize(init)) {
    log(LogLevel::Error, "Failed to initialize platform.");
    return 1;
  }

  const auto scenePath =
      std::filesystem::path(config_.projectRoot) / config_.project.defaultScene;
  const auto scene = Scene::load(scenePath);
  if (!scene) {
    log(LogLevel::Warn, "Could not load default scene; continuing with empty scene.");
  } else {
    log(LogLevel::Info, "Loaded scene: " + scene->name);
  }

  while (running_ && !platform_->shouldClose()) {
    platform_->pollEvents();
    platform_->beginFrame();
    platform_->clear(0.08f, 0.09f, 0.12f);
    platform_->endFrame();
  }

  platform_->shutdown();
  return 0;
}

}  // namespace vibe
