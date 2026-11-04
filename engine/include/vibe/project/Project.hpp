#pragma once

#include "vibe/core/Types.hpp"

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace vibe {

struct Project {
  static constexpr u32 kFormatVersion = 1;

  u32 formatVersion{kFormatVersion};
  std::string name;
  Version engineVersion{};
  std::filesystem::path defaultScene;

  static std::optional<Project> load(const std::filesystem::path& projectRoot);
};

std::filesystem::path defaultProjectManifestName();

}  // namespace vibe
