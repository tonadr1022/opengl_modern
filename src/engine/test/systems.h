#pragma once

#include <entt/entity/fwd.hpp>
#include <glm/fwd.hpp>
#include <vector>

namespace engine {
struct ModelData;
namespace component {
struct Transform;
}  // namespace component
}  // namespace engine

std::vector<std::string>& GetModelPaths();
void ModelViewerLoadModel(entt::registry& registry, engine::component::Transform& transform,
                          const engine::ModelData& model_data);
