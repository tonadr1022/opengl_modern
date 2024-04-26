#include "renderer.h"

#include <imgui.h>

#include "engine/renderer/material.h"
#include "engine/renderer/resource/mesh_manager.h"
#include "engine/renderer/resource/shader_manager.h"

namespace gfx {

namespace {
void ImGuiMenu() {
  ImGui::Begin("Renderer");
  if (ImGui::Button("Recompile Shaders")) {
    ShaderManager::RecompileShaders();
  }

  ImGui::End();
}
}  // namespace

namespace renderer {

struct DrawCommand {
  MeshID mesh_id;
  MaterialID material_id;
  glm::mat4 model_matrix;
};

std::vector<DrawCommand> draw_commands;

void Submit(DrawCommand& draw_command) { draw_commands.emplace_back(draw_command); }

void StartFrame() {
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_STENCIL_TEST);
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void EndFrame() { ImGuiMenu(); }

void AddBatchedMesh() {}

void ClearAllData() {}

}  // namespace renderer

}  // namespace gfx
