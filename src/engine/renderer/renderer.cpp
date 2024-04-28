#include "renderer.h"

#include <imgui.h>

#include "engine/pch.h"
#include "engine/renderer/gl/data_types.h"
#include "engine/renderer/gl/debug.h"
#include "engine/renderer/material.h"
#include "engine/renderer/resource/mesh_manager.h"
#include "engine/renderer/resource/shader_manager.h"

namespace gfx {

namespace renderer {

namespace {
void ImGuiMenu() {
  ImGui::Begin("Renderer");
  if (ImGui::Button("Recompile Shaders")) {
    ShaderManager::RecompileShaders();
  }

  ImGui::End();
}

struct DrawElementsIndirectCommand {
  uint count;
  uint instance_count;
  uint first_index;
  int base_vertex;
  uint base_instance;
};

struct VertexArrayIds {
  GLuint multidraw_id;
};

VertexArrayIds vao_ids;

}  // namespace

struct DrawCommand {
  MeshID mesh_id;
  MaterialID material_id;
  glm::mat4 model_matrix;
};

void InitVaos() {
  glGenVertexArrays(1, &vao_ids.multidraw_id);
  glBindVertexArray(vao_ids.multidraw_id);
  glEnableVertexArrayAttrib(vao_ids.multidraw_id, 0);
  glVertexAttribPointer(0, sizeof(glm::vec3), GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        reinterpret_cast<void*>((offsetof(Vertex, position))));
  glEnableVertexArrayAttrib(vao_ids.multidraw_id, 1);
  glVertexAttribPointer(1, sizeof(glm::vec3), GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        reinterpret_cast<void*>((offsetof(Vertex, normal))));
  glEnableVertexArrayAttrib(vao_ids.multidraw_id, 2);
  glVertexAttribPointer(2, sizeof(glm::vec3), GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        reinterpret_cast<void*>((offsetof(Vertex, tangents))));
  glEnableVertexArrayAttrib(vao_ids.multidraw_id, 3);
  glVertexAttribPointer(3, sizeof(glm::vec2), GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        reinterpret_cast<void*>((offsetof(Vertex, tex_coords))));
}

void Init() {
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(MessageCallback, nullptr);

  InitVaos();
}

std::vector<DrawCommand> draw_commands;

void Submit(DrawCommand& draw_command) { draw_commands.emplace_back(draw_command); }

void StartFrame() {
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_STENCIL_TEST);
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void EndFrame() { ImGuiMenu(); }

void ClearAllData() {}

}  // namespace renderer

}  // namespace gfx
