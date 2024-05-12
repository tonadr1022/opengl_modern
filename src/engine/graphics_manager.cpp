#include "graphics_manager.h"

#include <entt/entity/registry.hpp>

#include "engine/ecs/component/transform.h"
#include "engine/renderer/renderer.h"
#include "engine/resource/material_manager.h"
#include "engine/resource/resource.h"
#include "engine/scene.h"
#include "engine/window_manager.h"

namespace engine {

using component::ModelMatrix;
using component::Transform;
using gfx::Renderer;

void GraphicsManager::Init() {
  auto framebuffer_dims = WindowManager::Get().GetWindowDimensions();
  Renderer::Get().Init(framebuffer_dims);
}
void GraphicsManager::Shutdown() { Renderer::Get().Shutdown(); }

void GraphicsManager::StartFrame(Scene& scene) {
  Renderer::Get().StartFrame(scene.render_view_info);
}

void GraphicsManager::InitScene(Scene& /*scene*/) { Renderer::Get().Reset(); }

void update_model_matrices(entt::registry& registry) {
  ZoneScopedN("update model matrices");
  auto model_group = registry.view<Transform, ModelMatrix>();
  int i = 0;
  std::for_each(model_group.begin(), model_group.end(), [&i, model_group](auto entity) {
    auto [transform, model] = model_group.get<Transform, ModelMatrix>(entity);
    i++;
    if (transform.IsDirty()) {
      model.matrix = transform.CalculateModel();
      transform.SetModel();
    }
  });
}

void GraphicsManager::DrawOpaque(entt::registry& registry) {
  // auto par_group = registry.group<Transform>(entt::get<Parent>);
  // for (auto entity : par_group) {
  //   auto [world_transform, parent] = par_group.get<Transform, Parent>(entity);
  // }

  // TODO(tony): model matrix update in physics system along with scene graph
  update_model_matrices(registry);
  {
    ZoneScopedN("submit cmds");
    auto draw_cmd_group = registry.view<ModelMatrix, Mesh>();
    auto& renderer = Renderer::Get();
    draw_cmd_group.each([this, &renderer](auto& model_matrix, auto& mesh) {
      renderer.SubmitDrawCommand(model_matrix.matrix, mesh.mesh_handle, mesh.material_handle);
    });
  }

  Renderer::Get().RenderOpaqueObjects();
}

void GraphicsManager::EndFrame() { Renderer::Get().EndFrame(); }
}  // namespace engine
