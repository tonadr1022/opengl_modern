#include "systems.h"

#include <entt/entity/registry.hpp>
#include <fstream>

#include "engine/ecs/component/renderer_components.h"
#include "engine/ecs/component/transform.h"

void ModelViewerLoadModel(entt::registry &registry, engine::component::Transform &transform,
                          const std::vector<engine::component::MeshMaterial> &mesh_material) {
  for (const auto &mesh_mat : mesh_material) {
    auto ent = registry.create();
    registry.emplace<engine::component::MeshMaterial>(ent, mesh_mat);
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
