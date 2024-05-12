#include "systems.h"

#include <entt/entity/registry.hpp>
#include <fstream>

#include "engine/ecs/component/transform.h"
#include "engine/resource/resource.h"

void ModelViewerLoadModel(entt::registry &registry, engine::component::Transform &transform,
                          const engine::ModelData &model_data) {
  for (const auto &mesh : model_data.meshes) {
    auto ent = registry.create();
    registry.emplace<engine::Mesh>(ent, mesh);
    registry.emplace<engine::component::Transform>(ent, transform);
    registry.emplace<engine::component::ModelMatrix>(ent);
  }
}

namespace {
std::vector<std::string> paths;
bool loaded{false};
}  // namespace

std::vector<std::string> &GetModelPaths() {
  if (loaded) return paths;
  std::ifstream file("/home/tony/models_paths.txt");
  std::string line;
  while (std::getline(file, line)) {  // Read each line from the file
    paths.push_back(line);
  }
  loaded = true;
  return paths;
}
