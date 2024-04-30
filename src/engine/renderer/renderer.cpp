#include "renderer.h"

#include <imgui.h>

#include "engine/pch.h"
#include "engine/renderer/gl/buffer.h"
#include "engine/renderer/gl/data_types.h"
#include "engine/renderer/gl/debug.h"
#include "engine/renderer/material.h"
#include "engine/renderer/resource/mesh_manager.h"
#include "engine/renderer/resource/paths.h"
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
uint32_t user_draw_cmds_index{0};

std::map<MeshID, DrawElementsIndirectCommand> mesh_buffer_info;

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

void LoadShaders() {
  ShaderManager::AddShader("batch", {{GET_SHADER_PATH("batch.vs.glsl"), ShaderType::Vertex},
                                     {GET_SHADER_PATH("batch.fs.glsl"), ShaderType::Fragment}});
}

}  // namespace

void OnImGuiRender() {
  ImGui::Begin("Renderer");
  if (ImGui::Button("Recompile Shaders")) {
    ShaderManager::RecompileShaders();
  }
  ImGui::Text("Buffer Offsets: Vertex: %i, Index: %i", batch_vertex_buffer.offset,
              batch_element_buffer.offset);

  ImGui::End();
}

void SetBatchedObjectCount(uint32_t count) { user_draw_cmds.resize(count); }

void SubmitDrawCommand(const glm::mat4& model, MeshID mesh_id, MaterialID material_id) {
  user_draw_cmds[user_draw_cmds_index++] =
      UserDrawCommand{.mesh_id = mesh_id, .material_id = material_id, .model_matrix = model};
}

void Init() {
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(MessageCallback, nullptr);
  LoadShaders();

  InitBuffers();
  InitVaos();
}

void AddBatchedMesh(MeshID id, std::vector<Vertex>& vertices, std::vector<Index>& indices) {
  glNamedBufferSubData(batch_vertex_buffer.id, batch_vertex_buffer.offset,
                       sizeof(Vertex) * vertices.size(), vertices.data());
  glNamedBufferSubData(batch_element_buffer.id, batch_element_buffer.offset,
                       sizeof(Index) * indices.size(), indices.data());

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

  mesh_buffer_info[id] = cmd;

  // buffer is void* type
  batch_vertex_buffer.offset += vertices.size() * sizeof(Vertex);
  batch_element_buffer.offset += indices.size() * sizeof(Index);
}

void StartFrame() {
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_STENCIL_TEST);
  glClearColor(0.1, 0.1, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void DrawOpaqueHelper(MaterialID material_id, const std::vector<glm::mat4>& uniforms) {
  // for each mesh that has an instance count with this material,
  // set base instance and increment it by the number of instances
  // of the mesh that have this material, then add the command
  std::vector<DrawElementsIndirectCommand> commands;
  GLuint base_instance = 0;
  std::for_each(mesh_buffer_info.begin(), mesh_buffer_info.end(),
                [&commands, &base_instance](auto& cmd) {
                  if (cmd.second.instance_count != 0) {
                    cmd.second.base_instance = base_instance;
                    commands.push_back(cmd.second);
                    base_instance += cmd.second.instance_count;
                  }
                });

  // create draw indirect buffer
  Buffer draw_cmd_buffer;
  glCreateBuffers(1, &draw_cmd_buffer.id);
  glNamedBufferStorage(draw_cmd_buffer.id, std::span(commands).size_bytes(), commands.data(), 0);
  glBindBuffer(GL_DRAW_INDIRECT_BUFFER, draw_cmd_buffer.id);

  // clear instance count for future draw calls with diff materials
  for (auto& info : mesh_buffer_info) {
    info.second.instance_count = 0;
  }

  // create ssbo for uniforms (model matrices for each)
  Buffer ssbo_buffer;
  glCreateBuffers(1, &ssbo_buffer.id);
  glNamedBufferStorage(ssbo_buffer.id, std::span(uniforms).size_bytes(), uniforms.data(), 0);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo_buffer.id);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_buffer.id);

  auto batch_shader = ShaderManager::GetShader("batch");
  batch_shader->Bind();

  glBindVertexArray(batch_vao);
  // mode, type, offest ptr, command count, stride (0 since tightly packed)
  glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, commands.size(), 0);

  // TODO (tony): create buffer class with RAII
  glDeleteBuffers(1, &ssbo_buffer.id);
  glDeleteBuffers(1, &draw_cmd_buffer.id);
}

void RenderOpaqueObjects() {
  // sort user commands by material and mesh, must match mesh_buffer_info
  std::sort(std::execution::par, user_draw_cmds.begin(), user_draw_cmds.end(),
            [](const UserDrawCommand& lhs, const UserDrawCommand& rhs) {
              if (lhs.material_id != rhs.material_id) return lhs.material_id < rhs.material_id;
              return lhs.mesh_id < rhs.mesh_id;
            });

  // accumulate per-material draw commands and uniforms
  // for each material, push all the uniforms of meshes using the material
  // into a vector, and increase the instance count of each mesh.
  // When the next material doesn't match, draw with the current uniforms
  // and material id.
  std::vector<glm::mat4> uniforms;
  uniforms.reserve(user_draw_cmds.size());
  MaterialID curr_mat_id = user_draw_cmds[0].material_id;
  for (const auto& user_draw_cmd : user_draw_cmds) {
    if (user_draw_cmd.material_id != curr_mat_id) {
      // draw with this material and uniforms of objects that use this material
      DrawOpaqueHelper(curr_mat_id, uniforms);
      curr_mat_id = user_draw_cmd.material_id;
      uniforms.clear();
    }

    // push uniform and inc instance count of mesh of current command
    mesh_buffer_info[user_draw_cmd.mesh_id].instance_count++;
    uniforms.push_back(user_draw_cmd.model_matrix);
  }
  // if all materials are the same or the last commands used same material,
  // draw them
  if (uniforms.size() > 0) {
    DrawOpaqueHelper(curr_mat_id, uniforms);
  }

  // reset draw commands index
  user_draw_cmds_index = 0;
}

void EndFrame() {}

void ClearAllData() {}

}  // namespace renderer

}  // namespace gfx
