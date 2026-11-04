#include "vibe/project/Project.hpp"

#include "vibe/core/Log.hpp"

#include <fstream>
#include <nlohmann/json.hpp>

namespace vibe {

std::filesystem::path defaultProjectManifestName() {
  return "vibe.project.json";
}

std::optional<Project> Project::load(const std::filesystem::path& projectRoot) {
  const auto manifest = projectRoot / defaultProjectManifestName();
  if (!std::filesystem::exists(manifest)) {
    log(LogLevel::Error, "Missing project manifest: " + manifest.string());
    return std::nullopt;
  }

  std::ifstream in(manifest);
  if (!in) {
    log(LogLevel::Error, "Could not open project manifest.");
    return std::nullopt;
  }

  nlohmann::json doc;
  try {
    in >> doc;
  } catch (const std::exception& ex) {
    log(LogLevel::Error, std::string("Invalid JSON in project manifest: ") + ex.what());
    return std::nullopt;
  }

  Project project{};
  project.formatVersion = doc.value("formatVersion", Project::kFormatVersion);
  project.name = doc.value("name", "Untitled");
  const auto engineVersionText = doc.value("engineVersion", std::string("0.0.0"));
  project.engineVersion = Version::parse(engineVersionText);
  project.defaultScene = doc.value("defaultScene", std::string("scenes/main.scene.json"));

  if (project.formatVersion != Project::kFormatVersion) {
    log(LogLevel::Warn, "Project formatVersion differs from runtime; attempting load anyway.");
  }

  return project;
}

}  // namespace vibe
