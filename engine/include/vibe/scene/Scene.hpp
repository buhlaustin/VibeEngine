#pragma once

#include "vibe/core/Types.hpp"

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace vibe {

struct Transform {
  float x{0.f};
  float y{0.f};
  float z{0.f};
};

struct Entity {
  std::string id;
  std::string name;
  Transform transform{};
};

struct Scene {
  static constexpr u32 kFormatVersion = 1;

  u32 formatVersion{kFormatVersion};
  std::string name;
  std::vector<Entity> entities;

  static std::optional<Scene> load(const std::filesystem::path& scenePath);
};

}  // namespace vibe
