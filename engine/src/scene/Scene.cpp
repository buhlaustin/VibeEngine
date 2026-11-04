#include "vibe/scene/Scene.hpp"

#include "vibe/core/Log.hpp"

#include <fstream>
#include <nlohmann/json.hpp>

namespace vibe {

std::optional<Scene> Scene::load(const std::filesystem::path& scenePath) {
  if (!std::filesystem::exists(scenePath)) {
    log(LogLevel::Error, "Scene file not found: " + scenePath.string());
    return std::nullopt;
  }

  std::ifstream in(scenePath);
  if (!in) {
    log(LogLevel::Error, "Could not open scene file.");
    return std::nullopt;
  }

  nlohmann::json doc;
  try {
    in >> doc;
  } catch (const std::exception& ex) {
    log(LogLevel::Error, std::string("Invalid JSON in scene: ") + ex.what());
    return std::nullopt;
  }

  Scene scene{};
  scene.formatVersion = doc.value("formatVersion", Scene::kFormatVersion);
  scene.name = doc.value("name", "Scene");

  if (doc.contains("entities") && doc["entities"].is_array()) {
    for (const auto& entityJson : doc["entities"]) {
      Entity entity{};
      entity.id = entityJson.value("id", "");
      entity.name = entityJson.value("name", "Entity");
      if (entityJson.contains("transform") && entityJson["transform"].is_object()) {
        const auto& t = entityJson["transform"];
        entity.transform.x = t.value("x", 0.f);
        entity.transform.y = t.value("y", 0.f);
        entity.transform.z = t.value("z", 0.f);
      }
      scene.entities.push_back(std::move(entity));
    }
  }

  return scene;
}

}  // namespace vibe
