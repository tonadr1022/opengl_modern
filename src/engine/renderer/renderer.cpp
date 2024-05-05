#include "renderer.h"

#include <imgui.h>

#include <cstdint>

#include "engine/core/e_assert.h"
#include "engine/pch.h"
#include "engine/renderer/gl/buffer.h"
#include "engine/renderer/gl/debug.h"
#include "engine/renderer/gl/vertex_array.h"
#include "engine/renderer/material.h"
#include "engine/resource/material_manager.h"
#include "engine/resource/paths.h"
#include "engine/resource/shader_manager.h"
#include "engine/util/profiler.h"

namespace engine::gfx {

struct UserDrawCommand {
  MeshID mesh_id;
  MaterialID material_id;
  // glm::mat4 model_matrix;
  uint32_t model_matrix_index;
};

namespace {

struct DrawElementsIndirectCommand {
  uint32_t count;
  uint32_t instance_count;
  uint32_t first_index;
  int base_vertex;
  uint32_t base_instance;
};

constexpr uint32_t VertexBufferArrayMaxLength{10'000'000};
constexpr uint32_t IndexBufferArrayMaxLength{10'000'000};
constexpr uint32_t MaxDrawCommands{10'000'000};

struct CameraInfo {
  glm::mat4 view_matrix;
  glm::mat4 projection_matrix;
  glm::mat4 vp_matrix;
};

CameraInfo cam_info;

VertexArray batch_vao{};
Buffer batch_vertex_buffer;
Buffer batch_element_buffer;
Buffer batch_ssbo_buffer;
Buffer draw_indirect_buffer;

std::vector<UserDrawCommand> user_draw_cmds;
std::vector<glm::mat4> user_draw_cmd_model_matrices;
uint32_t user_draw_cmds_index{0};

std::map<MeshID, DrawElementsIndirectCommand> mesh_buffer_info;
uint32_t framebuffer_width{1}, frame_buffer_height{1};

std::vector<glm::mat4> uniforms;
void* batch_map_ptr{nullptr};

void InitBuffers() {
  batch_vertex_buffer =
      Buffer::Create(sizeof(Vertex) * VertexBufferArrayMaxLength, GL_DYNAMIC_STORAGE_BIT);
  batch_element_buffer =
      Buffer::Create(sizeof(Index) * IndexBufferArrayMaxLength, GL_DYNAMIC_STORAGE_BIT);
  draw_indirect_buffer =
      Buffer::Create(sizeof(DrawElementsIndirectCommand) * MaxDrawCommands, GL_DYNAMIC_STORAGE_BIT);
  batch_ssbo_buffer = Buffer::Create(sizeof(glm::mat4) * MaxDrawCommands, GL_DYNAMIC_STORAGE_BIT);
  // Buffer::Create(sizeof(glm::mat4) * MaxDrawCommands,
  //                GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
  // auto* batch_map_ptr = batch_ssbo_buffer.MapRange(0, sizeof(glm::mat4) * MaxDrawCommands,
  //                                GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
}

void InitVaos() {
  batch_vao = VertexArray::Create();
  batch_vao.EnableAttribute<float>(0, 3, offsetof(Vertex, position));
  batch_vao.EnableAttribute<float>(1, 3, offsetof(Vertex, normal));
  batch_vao.EnableAttribute<float>(2, 2, offsetof(Vertex, tex_coords));
  batch_vao.AttachVertexBuffer(batch_vertex_buffer.Id(), 0, 0, sizeof(Vertex));
  batch_vao.AttachElementBuffer(batch_element_buffer.Id());
}

void LoadShaders() {
  ShaderManager::AddShader("batch", {{GET_SHADER_PATH("batch.vs.glsl"), ShaderType::Vertex},
                                     {GET_SHADER_PATH("batch.fs.glsl"), ShaderType::Fragment}});
}
RendererStats stats{0};

}  // namespace

void Renderer::SetFrameBufferSize(uint32_t width, uint32_t height) {
  frame_buffer_height = height;
  framebuffer_width = width;
}

void Renderer::SetBatchedObjectCount(uint32_t count) {
  user_draw_cmd_model_matrices.resize(count);
  user_draw_cmds.resize(count);
}

void Renderer::SetStaticObjectCount(uint32_t count) {}

void Renderer::SubmitDrawCommand(const glm::mat4& model, MeshID mesh_id, MaterialID material_id) {
  user_draw_cmds[user_draw_cmds_index] = UserDrawCommand{
      .mesh_id = mesh_id, .material_id = material_id, .model_matrix_index = user_draw_cmds_index};
  user_draw_cmd_model_matrices[user_draw_cmds_index] = model;
  user_draw_cmds_index++;
}

const RendererStats& Renderer::GetStats() { return stats; }

void Renderer::Init() {
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(MessageCallback, nullptr);
  LoadShaders();
  InitBuffers();
  InitVaos();
}

void Renderer::Shutdown() { batch_ssbo_buffer.Unmap(); }

void Renderer::StartFrame(const RenderViewInfo& camera_matrices) {
  memset(&stats, 0, sizeof(stats));
  cam_info.view_matrix = camera_matrices.view_matrix;
  cam_info.projection_matrix = camera_matrices.projection_matrix;
  cam_info.vp_matrix = cam_info.projection_matrix * cam_info.view_matrix;

  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_STENCIL_TEST);
  glClearColor(0.1, 0.1, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void Renderer::AddBatchedMesh(MeshID id, std::vector<Vertex>& vertices,
                              std::vector<Index>& indices) {
  DrawElementsIndirectCommand cmd{};
  // number of elements
  cmd.count = static_cast<uint32_t>(indices.size());
  // location in the buffer of the first vertex of the mesh
  cmd.base_vertex = batch_vertex_buffer.Offset() / sizeof(Vertex);
  // first instance is of the sub buffer is used
  cmd.base_instance = 0;
  // location in the buffer of first index of the mesh
  cmd.first_index = batch_element_buffer.Offset() / sizeof(Index);
  // will be incremented when multiple draw calls use the same mesh
  cmd.instance_count = 0;

  mesh_buffer_info[id] = cmd;

  // need to be called after getting offset since this increases offset
  batch_vertex_buffer.SubData(sizeof(Vertex) * vertices.size(), vertices.data());
  batch_element_buffer.SubData(sizeof(Index) * indices.size(), indices.data());

  stats.meshes_in_memory++;
}

void DrawOpaqueHelper(MaterialID material_id, std::vector<glm::mat4>& uniforms) {
  PROFILE_FUNCTION();
  // for each mesh that has an instance count with this material,
  // set base instance and increment it by the number of instances
  // of the mesh that have this material, then add the command
  std::vector<DrawElementsIndirectCommand> commands;
  {
    PROFILE_SCOPE("iterate commands");
    GLuint base_instance = 0;
    std::for_each(mesh_buffer_info.begin(), mesh_buffer_info.end(),
                  [&commands, &base_instance](auto& cmd) {
                    if (cmd.second.instance_count != 0) {
                      cmd.second.base_instance = base_instance;
                      commands.push_back(cmd.second);
                      base_instance += cmd.second.instance_count;
                    }
                  });
  }

  {
    {
      PROFILE_SCOPE("Reset");
      batch_ssbo_buffer.Reset();
      draw_indirect_buffer.Reset();
    }
    {
      PROFILE_SCOPE("Subdata draw indirect");
      draw_indirect_buffer.SubData(sizeof(DrawElementsIndirectCommand) * commands.size(),
                                   commands.data());
    }
    {
      static int s = 0;
      s += uniforms.size();
      std::string name = "subdata batch ssbo" + std::to_string(uniforms.size()) + "  " +
                         std::to_string(batch_ssbo_buffer.Offset()) + "  " + std::to_string(s);
      PROFILE_SCOPE(name);
      // std::memcpy(batch_map_ptr, uniforms.data(), sizeof(glm::mat4) * uniforms.size());
      batch_ssbo_buffer.SubData(sizeof(glm::mat4) * uniforms.size(), uniforms.data());
    }
    {
      PROFILE_SCOPE("Bind dib");

      draw_indirect_buffer.Bind(GL_DRAW_INDIRECT_BUFFER);
    }
    {
      PROFILE_SCOPE("Bind ssbo");
      batch_ssbo_buffer.BindBase(GL_SHADER_STORAGE_BUFFER, 0);
    }
  }

  {
    PROFILE_SCOPE("Bind");
    // bind material
    auto& material = MaterialManager::GetMaterial(material_id);
    auto shader = ShaderManager::GetShader("batch");
    shader->SetVec3("material.diffuse", material.diffuse);
    shader->SetMat4("vp_matrix", cam_info.vp_matrix);
  }

  {
    PROFILE_SCOPE("Draw");
    // mode, type, offest ptr, command count, stride (0 since tightly packed)
    glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, commands.size(), 0);
    stats.multi_draw_calls++;
  }

  {
    PROFILE_SCOPE("reset");
    for (auto& info : mesh_buffer_info) {
      info.second.instance_count = 0;
    }
  }

  // clear instance count for future draw calls with diff materials
  // TODO (tony): create buffer class with RAII
}

void Renderer::RenderOpaqueObjects() {
  PROFILE_FUNCTION();
  auto shader = ShaderManager::GetShader("batch");
  EASSERT(shader.has_value());
  shader->Bind();
  batch_vao.Bind();

  // sort user commands by material and mesh, must match mesh_buffer_info
  std::sort(user_draw_cmds.begin(), user_draw_cmds.end(),
            [](const UserDrawCommand& lhs, const UserDrawCommand& rhs) {
              if (lhs.material_id != rhs.material_id) return lhs.material_id < rhs.material_id;
              return lhs.mesh_id < rhs.mesh_id;
            });

  // accumulate per-material draw commands and uniforms
  // for each material, push all the uniforms of meshes using the material
  // into a vector, and increase the instance count of each mesh.
  // When the next material doesn't match, draw with the current uniforms
  // and material id.
  uniforms.reserve(user_draw_cmds.size());
  MaterialID curr_mat_id = user_draw_cmds[0].material_id;
  for (const auto& user_draw_cmd : user_draw_cmds) {
    if (user_draw_cmd.material_id != curr_mat_id) {
      stats.material_swaps++;
      // draw with this material and uniforms of objects that use this material
      DrawOpaqueHelper(curr_mat_id, uniforms);
      curr_mat_id = user_draw_cmd.material_id;
      uniforms.clear();
    }

    // push uniform and inc instance count of mesh of current command
    mesh_buffer_info[user_draw_cmd.mesh_id].instance_count++;
    uniforms.push_back(user_draw_cmd_model_matrices[user_draw_cmd.model_matrix_index]);

    // uniforms.push_back(user_draw_cmd.model_matrix);
  }
  // if all materials are the same or the last commands used same material,
  // draw them
  if (!uniforms.empty()) {
    DrawOpaqueHelper(curr_mat_id, uniforms);
  }

  // reset draw commands index
  user_draw_cmds_index = 0;
  stats.meshes_drawn += user_draw_cmds.size();
  uniforms.clear();
}

void Renderer::EndFrame() {
  glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::Restart() { InitBuffers(); }

}  // namespace engine::gfx
