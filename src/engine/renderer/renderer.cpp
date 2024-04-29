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

struct UserDrawCommand {
  MeshID mesh_id;
  MaterialID material_id;
  glm::mat4 model_matrix;
};

namespace {

struct DrawElementsIndirectCommand {
  uint count;
  uint instance_count;
  uint first_index;
  int base_vertex;
  uint base_instance;
};

constexpr uint32_t VertexBufferArrayMaxLength{10'000'000};
constexpr uint32_t IndexBufferArrayMaxLength{10'000'000};

struct Buffer {
  uint32_t id{0};
  uint32_t offset{0};
};

uint32_t batch_vao{};
Buffer batch_vertex_buffer;
Buffer batch_element_buffer;

std::vector<UserDrawCommand> user_draw_cmds;
std::map<MeshID, DrawElementsIndirectCommand> mesh_draw_cmds;

void InitBuffers() {
  glCreateBuffers(1, &batch_vertex_buffer.id);
  glCreateBuffers(1, &batch_element_buffer.id);
  glNamedBufferStorage(batch_vertex_buffer.id, sizeof(Vertex) * VertexBufferArrayMaxLength, nullptr,
                       GL_DYNAMIC_STORAGE_BIT);
  glNamedBufferStorage(batch_element_buffer.id, sizeof(Index) * IndexBufferArrayMaxLength, nullptr,
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
  glVertexArrayVertexBuffer(batch_vao, 0, batch_vertex_buffer.id, 0, sizeof(Vertex));
  glVertexArrayElementBuffer(batch_vao, batch_element_buffer.id);
}

void ImGuiMenu() {
  ImGui::Begin("Renderer");
  if (ImGui::Button("Recompile Shaders")) {
    ShaderManager::RecompileShaders();
  }
  ImGui::Text("Buffer Offsets: Vertex: %i, Index: %i", batch_vertex_buffer.offset,
              batch_element_buffer.offset);

  ImGui::End();
}

}  // namespace

void LoadShaders() {}

void Init() {
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(MessageCallback, nullptr);

  InitBuffers();
  InitVaos();
  LoadShaders();
}

void AddBatchedMesh(MeshID id, std::vector<Vertex>& vertices, std::vector<Index>& indices) {
  glNamedBufferSubData(batch_vertex_buffer.id, batch_vertex_buffer.offset,
                       sizeof(Vertex) * vertices.size(), vertices.data());
  glNamedBufferSubData(batch_element_buffer.id, batch_element_buffer.offset,
                       sizeof(Index) * indices.size(), indices.data());
  // buffer is void* type
  batch_vertex_buffer.offset += vertices.size() * sizeof(Vertex);
  batch_element_buffer.offset += indices.size() * sizeof(Index);

  DrawElementsIndirectCommand cmd{};
  // number of elements
  cmd.count = static_cast<uint32_t>(indices.size());
  // location in the buffer of the first vertex of the mesh
  cmd.base_vertex = batch_vertex_buffer.offset / sizeof(Vertex);
  // first instance is of the sub buffer is used
  cmd.base_instance = 0;
  // location in the buffer of first index of the mesh
  cmd.first_index = batch_element_buffer.offset / sizeof(Index);
  // no instancing
  cmd.instance_count = 0;

  mesh_draw_cmds[id] = cmd;
}

void SubmitUserDrawCommand(UserDrawCommand& draw_command) {
  user_draw_cmds.emplace_back(draw_command);
}

void StartFrame() {
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_STENCIL_TEST);
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void EndFrame() {
  ImGuiMenu();

  Buffer draw_cmd_buffer;
  glCreateBuffers(1, &draw_cmd_buffer.id);
  glNamedBufferStorage(draw_cmd_buffer.id,
                       sizeof(DrawElementsIndirectCommand) * user_draw_cmds.size(), nullptr,
                       GL_DYNAMIC_STORAGE_BIT);

  for (auto& [mesh_id, material_id, model_matrix] : user_draw_cmds) {
  }

  // glBindVertexArray(batch_vao);
  // // mode, type, offest ptr, command count, stride (0 since tightly packed)
  // glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, user_draw_cmds.size(), 0);

  glDeleteBuffers(1, &draw_cmd_buffer.id);
}

void ClearAllData() {}

}  // namespace renderer

}  // namespace gfx
