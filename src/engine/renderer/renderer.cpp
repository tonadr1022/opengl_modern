#include "renderer.h"

#include <imgui.h>

#include <cstdint>

#include "engine/core/e_assert.h"
#include "engine/pch.h"
#include "engine/renderer/gl/buffer.h"
#include "engine/renderer/gl/debug.h"
#include "engine/renderer/gl/texture_2d.h"
#include "engine/renderer/material.h"
#include "engine/resource/paths.h"
#include "engine/resource/shader_manager.h"
#include "engine/util/profiler.h"
#include "renderer_types.h"

namespace engine::gfx {

namespace {

constexpr uint32_t VertexBufferArrayMaxLength{10'000'000};
constexpr uint32_t IndexBufferArrayMaxLength{10'000'000};
constexpr uint32_t MaxDrawCommands{10'000'000};

struct CameraInfo {
  glm::mat4 view_matrix;
  glm::mat4 projection_matrix;
  glm::mat4 vp_matrix;
};

}  // namespace

Renderer::Renderer(ShaderManager& shader_manager) : shader_manager_(shader_manager) {}
void Renderer::LoadShaders() {
  shader_manager_.AddShader("batch", {{GET_SHADER_PATH("batch.vs.glsl"), ShaderType::Vertex},
                                      {GET_SHADER_PATH("batch.fs.glsl"), ShaderType::Fragment}});
}

void Renderer::InitVaos() {
  // batch_vao_ = std::make_unique<VertexArray>();
  // batch_vao_->EnableAttribute<float>(0, 3, offsetof(Vertex, position));
  // batch_vao_->EnableAttribute<float>(1, 3, offsetof(Vertex, normal));
  // batch_vao_->EnableAttribute<float>(2, 2, offsetof(Vertex, tex_coords));
  // batch_vao_->AttachVertexBuffer(batch_vertex_buffer_->Id(), 0, 0, sizeof(Vertex));
  // batch_vao_->AttachElementBuffer(batch_element_buffer_->Id());
  glCreateVertexArrays(1, &batch_vao_);
  glEnableVertexArrayAttrib(batch_vao_, 0);
  glEnableVertexArrayAttrib(batch_vao_, 1);
  glEnableVertexArrayAttrib(batch_vao_, 2);
  glVertexArrayAttribFormat(batch_vao_, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
  glVertexArrayAttribFormat(batch_vao_, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
  glVertexArrayAttribFormat(batch_vao_, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, tex_coords));
  glVertexArrayAttribBinding(batch_vao_, 0, 0);
  glVertexArrayAttribBinding(batch_vao_, 1, 0);
  glVertexArrayAttribBinding(batch_vao_, 2, 0);
  glVertexArrayVertexBuffer(batch_vao_, 0, batch_vertex_buffer_->Id(), 0, sizeof(Vertex));
  glVertexArrayElementBuffer(batch_vao_, batch_element_buffer_->Id());
}

void Renderer::InitBuffers() {
  batch_vertex_buffer_ =
      std::make_unique<Buffer>(sizeof(Vertex) * VertexBufferArrayMaxLength, GL_DYNAMIC_STORAGE_BIT);
  batch_element_buffer_ =
      std::make_unique<Buffer>(sizeof(Index) * IndexBufferArrayMaxLength, GL_DYNAMIC_STORAGE_BIT);
  draw_indirect_buffer_ = std::make_unique<Buffer>(
      sizeof(DrawElementsIndirectCommand) * MaxDrawCommands, GL_DYNAMIC_STORAGE_BIT);
  batch_ssbo_uniform_buffer_ =
      std::make_unique<Buffer>(sizeof(BatchUniform) * MaxDrawCommands, GL_DYNAMIC_STORAGE_BIT);
  materials_buffer_ =
      std::make_unique<Buffer>(sizeof(BindlessMaterial) * MaxMaterials, GL_DYNAMIC_STORAGE_BIT);

  // Buffer::Create(sizeof(glm::mat4) * MaxDrawCommands,
  //                GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
  // auto* batch_map_ptr = batch_ssbo_buffer.MapRange(0, sizeof(glm::mat4) * MaxDrawCommands,
  //                                GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
}

void Renderer::SetFrameBufferSize(uint32_t width, uint32_t height) {
  frame_buffer_height_ = height;
  framebuffer_width_ = width;
}

void Renderer::SetBatchedObjectCount(uint32_t count) {
  draw_cmd_mesh_ids_.reserve(count);
  draw_cmd_uniforms_.reserve(count);
}

void Renderer::SubmitDrawCommand(const glm::mat4& model, MeshID mesh_id, MaterialID material_id) {
  draw_cmd_mesh_ids_.emplace_back(mesh_id);
  draw_cmd_uniforms_.emplace_back(model, material_id);
  // user_draw_cmds_[user_draw_cmds_index_] = UserDrawCommand{
  //     .mesh_id = mesh_id, .material_id = material_id, .model_matrix_index =
  //     user_draw_cmds_index_};
  // user_draw_cmd_model_matrices_[user_draw_cmds_index_] = model;
  // user_draw_cmds_index_++;
}

const RendererStats& Renderer::GetStats() { return stats_; }

void Renderer::Init() {
  glDebugMessageCallback(MessageCallback, nullptr);
  LoadShaders();
  spdlog::info("loaded shaders");
  InitBuffers();
  InitVaos();
}

void Renderer::Shutdown() {}

void Renderer::StartFrame(const RenderViewInfo& camera_matrices) {
  memset(&stats_, 0, sizeof(stats_));
  view_matrix_ = camera_matrices.view_matrix;
  projection_matrix_ = camera_matrices.projection_matrix;
  vp_matrix_ = projection_matrix_ * view_matrix_;
  draw_cmd_uniforms_.clear();
  draw_cmd_mesh_ids_.clear();
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_STENCIL_TEST);
  glClearColor(0.1, 0.1, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

MeshID Renderer::AddBatchedMesh(std::vector<Vertex>& vertices, std::vector<Index>& indices) {
  DrawElementsIndirectCommand cmd{};
  // number of elements
  cmd.count = static_cast<uint32_t>(indices.size());
  // location in the buffer of the first vertex of the mesh
  cmd.base_vertex = batch_vertex_buffer_->Offset() / sizeof(Vertex);
  // first instance is of the sub buffer is used
  cmd.base_instance = 0;
  // location in the buffer of first index of the mesh
  cmd.first_index = batch_element_buffer_->Offset() / sizeof(Index);
  // will be incremented when multiple draw calls use the same mesh
  cmd.instance_count = 0;

  // FOR NOW, renderer controls mesh ids since it allocates meshes linearly
  MeshID id = mesh_buffer_info_.size();

  // NEED TO BE CALLED AFTER BASE_VERTEX AND FIRST_INDEX ASSIGNMENT SINCE THIS INCREMENTS THEM
  batch_vertex_buffer_->SubData(sizeof(Vertex) * vertices.size(), vertices.data());
  batch_element_buffer_->SubData(sizeof(Index) * indices.size(), indices.data());

  stats_.num_meshes++;
  // TODO(tony): change to vector so renderer controls ids
  return id;
}

// void Renderer::DrawOpaqueHelper(MaterialID material_id, std::vector<glm::mat4>& uniforms) {
//   PROFILE_FUNCTION();
//   // for each mesh that has an instance count with this material,
//   // set base instance and increment it by the number of instances
//   // of the mesh that have this material, then add the command
//   std::vector<DrawElementsIndirectCommand> commands;
//   {
//     PROFILE_SCOPE("iterate commands");
//     GLuint base_instance = 0;
//     std::for_each(mesh_buffer_info_.begin(), mesh_buffer_info_.end(),
//                   [&commands, &base_instance](auto& cmd) {
//                     if (cmd.second.instance_count != 0) {
//                       cmd.second.base_instance = base_instance;
//                       commands.push_back(cmd.second);
//                       base_instance += cmd.second.instance_count;
//                     }
//                   });
//   }
//
//   {
//     {
//       PROFILE_SCOPE("Reset");
//       batch_ssbo_buffer_->ResetOffset();
//       draw_indirect_buffer_->ResetOffset();
//     }
//     {
//       PROFILE_SCOPE("Subdata draw indirect");
//       draw_indirect_buffer_->SubData(sizeof(DrawElementsIndirectCommand) * commands.size(),
//                                      commands.data());
//     }
//     {
//       static int s = 0;
//       s += uniforms.size();
//       std::string name = "subdata batch ssbo" + std::to_string(uniforms.size()) + "  " +
//                          std::to_string(batch_ssbo_buffer_->Offset()) + "  " + std::to_string(s);
//       PROFILE_SCOPE(name);
//       // std::memcpy(batch_map_ptr, uniforms.data(), sizeof(glm::mat4) * uniforms.size());
//       batch_ssbo_buffer_->SubData(sizeof(glm::mat4) * uniforms.size(), uniforms.data());
//     }
//     {
//       PROFILE_SCOPE("Bind dib");
//
//       draw_indirect_buffer_->Bind(GL_DRAW_INDIRECT_BUFFER);
//     }
//     {
//       PROFILE_SCOPE("Bind ssbo");
//       batch_ssbo_buffer_->BindBase(GL_SHADER_STORAGE_BUFFER, 0);
//     }
//   }
//
//   {
//     PROFILE_SCOPE("Bind");
//     // bind material
//     // auto& material = MaterialManager::GetMaterial(material_id);
//     auto shader = shader_manager_.GetShader("batch");
//     shader->SetVec3("material.diffuse", glm::vec3{1, 0, 0});
//     shader->SetMat4("vp_matrix", vp_matrix_);
//   }
//
//   {
//     PROFILE_SCOPE("Draw");
//     // mode, type, offest ptr, command count, stride (0 since tightly packed)
//     glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, commands.size(), 0);
//     stats_.multi_draw_calls++;
//   }
//
//   {
//     PROFILE_SCOPE("reset");
//     for (auto& info : mesh_buffer_info_) {
//       info.second.instance_count = 0;
//     }
//   }
//
//   // clear instance count for future draw calls with diff materials
//   // TODO (tony): create buffer class with RAII
// }

void Renderer::RenderOpaqueObjects() {
  PROFILE_FUNCTION();
  auto shader = shader_manager_.GetShader("batch");
  EASSERT(shader.has_value());
  shader->Bind();
  glBindVertexArray(batch_vao_);
  batch_ssbo_uniform_buffer_->ResetOffset();
  batch_ssbo_uniform_buffer_->SubData(sizeof(BatchUniform) * draw_cmd_uniforms_.size(),
                                      draw_cmd_uniforms_.data());

  std::vector<DrawElementsIndirectCommand> cmds;
  cmds.reserve(draw_cmd_uniforms_.size());
  DrawElementsIndirectCommand cmd;
  size_t base_instance{0};

  size_t num_draw_cmds = draw_cmd_uniforms_.size();

  for (size_t i = 0; i < num_draw_cmds; i++) {
    auto& draw_cmd_info = mesh_buffer_info_[i];
    cmd.base_instance = base_instance;
    cmd.base_vertex = draw_cmd_info.base_vertex;
    cmd.instance_count = 0;
    cmd.count = draw_cmd_info.count;
    cmd.first_index = draw_cmd_info.first_index;
    cmds.emplace_back(cmd);
  }
  draw_indirect_buffer_->ResetOffset();
  draw_indirect_buffer_->SubData(sizeof(DrawElementsIndirectCommand) * cmds.size(), cmds.data());

  // std::memcpy(batch_map_ptr, uniforms.data(), sizeof(glm::mat4) * uniforms.size());

  draw_indirect_buffer_->Bind(GL_DRAW_INDIRECT_BUFFER);
  batch_ssbo_uniform_buffer_->BindBase(GL_SHADER_STORAGE_BUFFER, 0);
  materials_buffer_->BindBase(GL_SHADER_STORAGE_BUFFER, 1);
  shader->SetMat4("vp_matrix", vp_matrix_);

  // mode, type, offest ptr, command count, stride (0 since tightly packed)
  glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, cmds.size(), 0);
  stats_.multi_draw_calls++;

  for (auto& info : mesh_buffer_info_) {
    // info.second.instance_count = 0;
  }

  // // sort user commands by material and mesh, must match mesh_buffer_info
  // std::sort(user_draw_cmds_.begin(), user_draw_cmds_.end(),
  //           [](const UserDrawCommand& lhs, const UserDrawCommand& rhs) {
  //             if (lhs.material_id != rhs.material_id) return lhs.material_id < rhs.material_id;
  //             return lhs.mesh_id < rhs.mesh_id;
  //           });
  //
  // // accumulate per-material draw commands and uniforms
  // // for each material, push all the uniforms of meshes using the material
  // // into a vector, and increase the instance count of each mesh.
  // // When the next material doesn't match, draw with the current uniforms
  // // and material id.
  // uniforms_.reserve(user_draw_cmds_.size());
  //
  // MaterialID curr_mat_id = user_draw_cmds_[0].material_id;
  // for (const auto& user_draw_cmd : user_draw_cmds_) {
  //   if (user_draw_cmd.material_id != curr_mat_id) {
  //     stats_.material_swaps++;
  //     // draw with this material and uniforms of objects that use this material
  //     DrawOpaqueHelper(curr_mat_id, uniforms_);
  //     curr_mat_id = user_draw_cmd.material_id;
  //     uniforms_.clear();
  //   }
  //
  //   // push uniform and inc instance count of mesh of current command
  //   mesh_buffer_info_[user_draw_cmd.mesh_id].instance_count++;
  //   uniforms_.push_back(user_draw_cmd_model_matrices_[user_draw_cmd.model_matrix_index]);
  //
  //   // uniforms.push_back(user_draw_cmd.model_matrix);
  // }
  // // if all materials are the same or the last commands used same material,
  // // draw them
  // if (!uniforms_.empty()) {
  //   DrawOpaqueHelper(curr_mat_id, uniforms_);
  // }
  //
  // // reset draw commands index
  // user_draw_cmds_index_ = 0;
  // stats_.meshes_drawn += user_draw_cmds_.size();
  // uniforms_.clear();
}

void Renderer::EndFrame() {
  glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::Restart() { InitBuffers(); }

MaterialID Renderer::AddMaterial(const MaterialData& material) {
  EASSERT_MSG(materials_buffer_ != nullptr, "buffer not initialized");
  // assign handles so material samplers, properties can be accessed in shaders
  BindlessMaterial bindless_mat;
  if (material.albedo_texture != nullptr)
    bindless_mat.albedo_map_handle = material.albedo_texture->BindlessHandle();
  if (material.normal_texture != nullptr)
    bindless_mat.normal_map_handle = material.normal_texture->BindlessHandle();
  if (material.metalness_texture != nullptr)
    bindless_mat.metalness_map_handle = material.metalness_texture->BindlessHandle();
  if (material.ao_texture != nullptr)
    bindless_mat.ao_map_handle = material.ao_texture->BindlessHandle();
  if (material.roughness_texture != nullptr)
    bindless_mat.roughness_map_handle = material.roughness_texture->BindlessHandle();
  bindless_mat.base_color = material.base_color;
  // id is the index into the material buffer.
  MaterialID id = materials_buffer_->SubData(sizeof(BindlessMaterial), &bindless_mat);
  return id;
}

}  // namespace engine::gfx
