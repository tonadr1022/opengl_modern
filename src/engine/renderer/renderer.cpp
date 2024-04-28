#include "renderer.h"

#include <imgui.h>

#include "engine/pch.h"
#include "engine/renderer/gl/buffer.h"
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

uint32_t batch_vao{};
uint32_t batch_vertex_buffer{};
uint32_t batch_element_buffer{};

}  // namespace

struct UserDrawCommand {
  MeshID mesh_id;
  MaterialID material_id;
  glm::mat4 model_matrix;
};

void LoadShaders() {}

void InitBuffers() {
  uint32_t vertex_buffer_size{10'000'000};
  uint32_t element_buffer_size{10'000'000};
  glCreateBuffers(1, &batch_vertex_buffer);
  glCreateBuffers(1, &batch_element_buffer);
  glNamedBufferStorage(batch_vertex_buffer, sizeof(Vertex) * vertex_buffer_size, nullptr,
                       GL_DYNAMIC_STORAGE_BIT);
  glNamedBufferStorage(batch_element_buffer, sizeof(Index) * element_buffer_size, nullptr,
                       GL_DYNAMIC_STORAGE_BIT);
}

void InitVaos() {
  ///////////////////// Batched VAO ///////////////////////////////////////////////
  glCreateVertexArrays(1, &batch_vao);
  // enable the attributes
  glEnableVertexArrayAttrib(batch_vao, 0);
  glEnableVertexArrayAttrib(batch_vao, 1);
  glEnableVertexArrayAttrib(batch_vao, 2);
  // specify format of vertex data.
  glVertexArrayAttribFormat(batch_vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
  glVertexArrayAttribFormat(batch_vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
  glVertexArrayAttribFormat(batch_vao, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, tex_coords));
  // specify where vbos need to be bound
  glVertexArrayAttribBinding(batch_vao, 0, 0);
  glVertexArrayAttribBinding(batch_vao, 1, 0);
  glVertexArrayAttribBinding(batch_vao, 2, 0);

  // attach buffers
  glVertexArrayVertexBuffer(batch_vao, 0, batch_vertex_buffer, 0, sizeof(Vertex));
  glVertexArrayElementBuffer(batch_vao, batch_element_buffer);
}

void Init() {
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(MessageCallback, nullptr);
  InitBuffers();
  InitVaos();
  LoadShaders();
}

std::vector<UserDrawCommand> draw_commands;

void SubmitUserDrawCommand(UserDrawCommand& draw_command) {
  draw_commands.emplace_back(draw_command);
}

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
