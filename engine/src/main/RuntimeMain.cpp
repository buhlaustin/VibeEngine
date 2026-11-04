#include "vibe/core/Application.hpp"
#include "vibe/core/Log.hpp"
#include "vibe/project/Project.hpp"

#include <filesystem>
#include <iostream>

namespace {

std::filesystem::path findProjectRoot(const std::filesystem::path& start) {
  auto current = std::filesystem::absolute(start);
  while (true) {
    if (std::filesystem::exists(current / vibe::defaultProjectManifestName())) {
      return current;
    }
    if (!current.has_parent_path() || current.parent_path() == current) {
      break;
    }
    current = current.parent_path();
  }
  return {};
}

}  // namespace

int main(int argc, char** argv) {
  std::filesystem::path hint = std::filesystem::current_path();
  if (argc >= 2) {
    hint = argv[1];
  }

  const auto projectRoot = findProjectRoot(hint);
  if (projectRoot.empty()) {
    vibe::log(vibe::LogLevel::Error, "No vibe.project.json found in path hierarchy.");
    return 1;
  }

  const auto project = vibe::Project::load(projectRoot);
  if (!project) {
    return 1;
  }

  vibe::log(vibe::LogLevel::Info, "Vibe runtime — project: " + project->name);
  vibe::Application app(vibe::RuntimeConfig{projectRoot.string(), *project});
  return app.run();
}
