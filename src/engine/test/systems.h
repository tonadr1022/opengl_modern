#pragma once

#include <entt/entity/fwd.hpp>
#include <glm/fwd.hpp>
#include <vector>

namespace engine {
namespace component {
struct Transform;
struct MeshMaterial;
}  // namespace component
}  // namespace engine

std::vector<std::string>& GetModelPaths();
void ModelViewerLoadModel(entt::registry& registry, engine::component::Transform& transform,
                          const std::vector<engine::component::MeshMaterial>& mesh_materials);
