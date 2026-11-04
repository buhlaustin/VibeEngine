#pragma once

#include "vibe/core/Types.hpp"

#include <memory>
#include <string_view>

namespace vibe {

struct PlatformInit {
  std::string_view windowTitle;
  i32 width{1280};
  i32 height{720};
};

class Platform {
 public:
  virtual ~Platform() = default;

  virtual bool initialize(const PlatformInit& init) = 0;
  virtual void shutdown() = 0;

  virtual void pollEvents() = 0;
  virtual bool shouldClose() const = 0;

  virtual void beginFrame() = 0;
  virtual void endFrame() = 0;

  virtual void clear(float r, float g, float b) = 0;
};

std::unique_ptr<Platform> createPlatform();

}  // namespace vibe
