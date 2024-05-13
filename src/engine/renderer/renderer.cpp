#include "renderer.h"

#include <imgui.h>

#include "engine/renderer/light.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <cstdint>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "engine/core/base.h"
#include "engine/core/e_assert.h"
#include "engine/pch.h"
#include "engine/renderer/gl/buffer.h"
#include "engine/renderer/gl/debug.h"
#include "engine/renderer/gl/texture_2d.h"
#include "engine/renderer/material.h"
#include "engine/resource/paths.h"
#include "engine/resource/shader_manager.h"
#include "renderer_types.h"

namespace engine::gfx {

Renderer* Renderer::instance_{nullptr};
Renderer& Renderer::Get() { return *instance_; }
Renderer::Renderer() {
  EASSERT_MSG(instance_ == nullptr, "Cannot create two renderers .");
  instance_ = this;
}

void Renderer::LoadShaders() {
  auto& manager = ShaderManager::Get();
  // set ubo for relevant shaders
  GLuint ubo_block_index;
  GLuint id;
  id = manager
           .AddShader("batch", {{GET_SHADER_PATH("batch.vs.glsl"), ShaderType::Vertex},
                                {GET_SHADER_PATH("batch.fs.glsl"), ShaderType::Fragment}})
           ->Id();
  // get index
  ubo_block_index = glGetUniformBlockIndex(id, "Matrices");
  // assign binding point
  glUniformBlockBinding(id, ubo_block_index, 0);
  // TODO(tony): deferred rendering

  // id = manager
  //          .AddShader("gbuffer", {{GET_SHADER_PATH("gbuffer.vs.glsl"), ShaderType::Vertex},
  //                                 {GET_SHADER_PATH("gbuffer.fs.glsl"), ShaderType::Fragment}})
  //          ->Id();
  // ubo_block_index = glGetUniformBlockIndex(id, "Matrices");
  // glUniformBlockBinding(id, ubo_block_index, 0);
}

Renderer::~Renderer() = default;

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
}

void Renderer::InitBuffers() {
  // GL_DYNAMIC_STORAGE_BIT - contents can be update after creation
  batch_vertex_buffer_ = std::make_unique<Buffer>(sizeof(Vertex) * kVertexBufferArrayMaxLength,
                                                  GL_DYNAMIC_STORAGE_BIT);
  batch_element_buffer_ =
      std::make_unique<Buffer>(sizeof(Index) * kIndexBufferArrayMaxLength, GL_DYNAMIC_STORAGE_BIT);
  draw_indirect_buffer_ = std::make_unique<Buffer>(
      sizeof(DrawElementsIndirectCommand) * kMaxDrawCommands, GL_DYNAMIC_STORAGE_BIT);
  batch_uniform_ssbo_ =
      std::make_unique<Buffer>(sizeof(BatchUniform) * kMaxDrawCommands, GL_DYNAMIC_STORAGE_BIT);
  materials_buffer_ = std::make_unique<Buffer>(sizeof(BindlessMaterial) * kMaxMaterials,
                                               GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT);
  // ubo for vp matrix for now. May add other matrices/uniforms
  // TODO(tony): don't hardcode matrix
  shader_uniform_ubo_ = std::make_unique<Buffer>(64, GL_DYNAMIC_STORAGE_BIT);
  // lights
  light_ssbo_ = std::make_unique<Buffer>(sizeof(PointLight) * kMaxLights, GL_DYNAMIC_STORAGE_BIT);

  // glCreateBuffers(1, &materials_buffer_);
  // glNamedBufferStorage(materials_buffer_, sizeof(BindlessMaterial) * MaxMaterials, nullptr,
  //                      GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT);

  // TODO(tony): persistent map buffer
  // Buffer::Create(sizeof(glm::mat4) * MaxDrawCommands,
  //                GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
  // auto* batch_map_ptr = batch_ssbo_buffer.MapRange(0, sizeof(glm::mat4) * MaxDrawCommands,
  //                                GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
}

void Renderer::OnFrameBufferResize(uint32_t width, uint32_t height) {
  framebuffer_dims_.y = height;
  framebuffer_dims_.x = width;
  glViewport(0, 0, framebuffer_dims_.x, framebuffer_dims_.y);
  ResetFrameBuffers();
}

void Renderer::SetBatchedObjectCount(uint32_t count) {
  draw_cmd_mesh_ids_.reserve(count);
  draw_cmd_uniforms_.reserve(count);
}

void Renderer::SubmitDrawCommand(const glm::mat4& model, AssetHandle mesh_handle,
                                 AssetHandle material_handle) {
  draw_cmd_mesh_ids_.emplace_back(mesh_handle);
  draw_cmd_uniforms_.emplace_back(model, material_handle);
  // user_draw_cmds_[user_draw_cmds_index_] = UserDrawCommand{
  //     .mesh_id = mesh_id, .material_id = material_id, .model_matrix_index =
  //     user_draw_cmds_index_};
  // user_draw_cmd_model_matrices_[user_draw_cmds_index_] = model;
  // user_draw_cmds_index_++;
}

const RendererStats& Renderer::GetStats() { return stats_; }

void Renderer::OnImGuiRender() {
  ImGui::Begin("Renderer", nullptr, ImGuiWindowFlags_NoNavFocus);
  // if (ImGui::TreeNodeEx("Stats", ImGuiTreeNodeFlags_DefaultOpen)) {
  if (ImGui::CollapsingHeader("Metrics", ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::Text("Vertices: %i", stats_.vertices);
    ImGui::Text("Indices: %i", stats_.indices);
    ImGui::Text("MultiDrawCalls: %i", stats_.multi_draw_calls);
    ImGui::Text("Multidraw commands Buffer: %i", stats_.multi_draw_cmds_buffer_count);
    ImGui::Text("Num Meshes: %i", stats_.num_meshes);
    ImGui::Text("VBO offset: %i, %f", batch_vertex_buffer_->Offset(),
                static_cast<float>(batch_vertex_buffer_->Offset()) / kVertexBufferArrayMaxLength);
    ImGui::Text("EBO Offset: %i, %f", batch_element_buffer_->Offset(),
                static_cast<float>(batch_element_buffer_->Offset()) / kIndexBufferArrayMaxLength);
  }

  if (ImGui::CollapsingHeader("PBR", ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::Checkbox("Override Albedo", &internal_settings_.use_override_albedo);
    ImGui::SameLine();
    ImGui::Checkbox("Override Metallic", &internal_settings_.use_override_metallic);
    ImGui::Checkbox("Override Roughness", &internal_settings_.use_override_roughness);
    ImGui::DragFloat3("Albedo Override", &internal_settings_.albedo_override.x, 0.01, 0, 1);
    ImGui::DragFloat("Metallic Override", &internal_settings_.metallic_roughness_override.x, .01, 0,
                     1);
    ImGui::DragFloat("Roughness Override", &internal_settings_.metallic_roughness_override.y, .01,
                     0, 1);
  }

  // if (ImGui::CollapsingHeader("Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
  // }
  ImGui::End();
}

void Renderer::ResetFrameBuffers() {
  // attach textures to the gbuffer FBO
  // albedo
  if (g_albedo_tex_) glDeleteTextures(1, &g_albedo_tex_);
  glCreateTextures(GL_TEXTURE_2D, 1, &g_albedo_tex_);
  glTextureStorage2D(g_albedo_tex_, 1, GL_RGBA8, framebuffer_dims_.x, framebuffer_dims_.y);
  glNamedFramebufferTexture(g_buffer_, GL_COLOR_ATTACHMENT2, g_albedo_tex_, 0);

  // normal
  // TODO(tony): switch texture format and do sphere map calculation
  if (g_normal_tex_) glDeleteTextures(1, &g_normal_tex_);
  glCreateTextures(GL_TEXTURE_2D, 1, &g_normal_tex_);
  glTextureStorage2D(g_normal_tex_, 1, GL_RGBA16F, framebuffer_dims_.x, framebuffer_dims_.y);
  glNamedFramebufferTexture(g_buffer_, GL_COLOR_ATTACHMENT1, g_normal_tex_, 0);

  // roughness metallic ambient occlusion
  if (g_rma_tex_) glDeleteTextures(1, &g_rma_tex_);
  glCreateTextures(GL_TEXTURE_2D, 1, &g_rma_tex_);
  // GL_RGB10_A2: 10 bits for rgb, 2 for alpha, since alpha not used.
  glTextureStorage2D(g_rma_tex_, 1, GL_RGB10_A2, framebuffer_dims_.x, framebuffer_dims_.y);
  glNamedFramebufferTexture(g_buffer_, GL_COLOR_ATTACHMENT0, g_rma_tex_, 0);

  // depth
  glCreateTextures(GL_TEXTURE_2D, 1, &g_depth_tex_);
  glTextureStorage2D(g_depth_tex_, 1, GL_DEPTH_COMPONENT32F, framebuffer_dims_.x,
                     framebuffer_dims_.y);
  glNamedFramebufferTexture(g_buffer_, GL_DEPTH_ATTACHMENT, g_depth_tex_, 0);

  if (!glCheckNamedFramebufferStatus(g_buffer_, GL_FRAMEBUFFER)) {
    spdlog::error("g buffer incomplete.");
  }
  // specify color buffers to draw into
  GLenum bufs[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
  glNamedFramebufferDrawBuffers(g_buffer_, 3, bufs);
}

void Renderer::InitFrameBuffers() {
  glCreateFramebuffers(1, &g_buffer_);
  ResetFrameBuffers();
}

void Renderer::InitTempLights() {
  // light_ssbo_->SubData(sizeof(PointLight), void *data)
}
void Renderer::Init(glm::ivec2 framebuffer_dims) {
  framebuffer_dims_ = framebuffer_dims;
  glViewport(0, 0, framebuffer_dims_.x, framebuffer_dims_.y);
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(MessageCallback, nullptr);
  LoadShaders();
  InitBuffers();
  InitVaos();
  InitFrameBuffers();
  InitTempLights();

  glVertexArrayVertexBuffer(batch_vao_, 0, batch_vertex_buffer_->Id(), 0, sizeof(Vertex));
  glVertexArrayElementBuffer(batch_vao_, batch_element_buffer_->Id());
}

void Renderer::Shutdown() {}

void Renderer::StartFrame(const RenderViewInfo& view_info) {
  // can't memset since num meshes is across frames
  stats_.indices = 0;
  stats_.multi_draw_calls = 0;
  stats_.multi_draw_cmds_buffer_count = 0;
  stats_.vertices = 0;

  // shader matrix uniforms
  view_matrix_ = view_info.view_matrix;
  projection_matrix_ = view_info.projection_matrix;
  vp_matrix_ = projection_matrix_ * view_matrix_;

  shader_uniform_ubo_->BindBase(GL_UNIFORM_BUFFER, 0);
  shader_uniform_ubo_->ResetOffset();
  shader_uniform_ubo_->SubData(64, glm::value_ptr(vp_matrix_));

  draw_cmd_uniforms_.clear();
  draw_cmd_mesh_ids_.clear();
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_STENCIL_TEST);
  glClearColor(0.6, 0.6, 0.6, 1.0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

AssetHandle Renderer::AddBatchedMesh(std::vector<Vertex>& vertices, std::vector<Index>& indices) {
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
  AssetHandle id = draw_elements_indirect_cmds_.size();
  draw_elements_indirect_cmds_.emplace_back(cmd);

  // NEED TO BE CALLED AFTER BASE_VERTEX AND FIRST_INDEX ASSIGNMENT SINCE THIS INCREMENTS THEM
  batch_vertex_buffer_->SubData(sizeof(Vertex) * vertices.size(), vertices.data());
  batch_element_buffer_->SubData(sizeof(Index) * indices.size(), indices.data());

  stats_.num_meshes++;
  return id;
}

void Renderer::RenderOpaqueObjects() {
  ZoneScopedN("Render RenderOpaqueObjects");
  auto shader = ShaderManager::Get().GetShader("batch");
  shader->Bind();
  glBindVertexArray(batch_vao_);
  batch_uniform_ssbo_->ResetOffset();
  batch_uniform_ssbo_->SubData(sizeof(BatchUniform) * draw_cmd_uniforms_.size(),
                               draw_cmd_uniforms_.data());

  std::vector<DrawElementsIndirectCommand> cmds;
  cmds.reserve(draw_cmd_uniforms_.size());
  DrawElementsIndirectCommand cmd;
  size_t base_instance{0};

  EASSERT_MSG(draw_cmd_mesh_ids_.size() == draw_cmd_uniforms_.size(), "need same size");
  for (auto draw_cmd_mesh_id : draw_cmd_mesh_ids_) {
    auto& draw_cmd_info = draw_elements_indirect_cmds_[draw_cmd_mesh_id];
    cmd.base_instance = base_instance;
    cmd.base_vertex = draw_cmd_info.base_vertex;
    cmd.instance_count = 1;
    cmd.count = draw_cmd_info.count;
    cmd.first_index = draw_cmd_info.first_index;
    stats_.indices += cmd.count;
    stats_.multi_draw_cmds_buffer_count++;
    cmds.emplace_back(cmd);
  }

  draw_indirect_buffer_->ResetOffset();
  draw_indirect_buffer_->SubData(sizeof(DrawElementsIndirectCommand) * cmds.size(), cmds.data());

  // std::memcpy(batch_map_ptr, uniforms.data(), sizeof(glm::mat4) * uniforms.size());

  draw_indirect_buffer_->Bind(GL_DRAW_INDIRECT_BUFFER);
  batch_uniform_ssbo_->BindBase(GL_SHADER_STORAGE_BUFFER, 0);
  // glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, materials_buffer_);
  materials_buffer_->BindBase(GL_SHADER_STORAGE_BUFFER, 1);

  // mode, type, offest ptr, command count, stride (0 since tightly packed)
  glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, cmds.size(), 0);
  stats_.multi_draw_calls++;
}

void Renderer::EndFrame() {
  glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::Reset() {
  memset(&stats_, 0, sizeof(stats_));
  // reset buffers to allocate new scene
  draw_elements_indirect_cmds_.clear();
  material_id_to_index_.clear();
  draw_cmd_mesh_ids_.clear();
  batch_vertex_buffer_->ResetOffset();
  batch_element_buffer_->ResetOffset();
  draw_indirect_buffer_->ResetOffset();
  batch_uniform_ssbo_->ResetOffset();
  materials_buffer_->ResetOffset();
}

AssetHandle Renderer::AddMaterial(const MaterialData& material) {
  EASSERT_MSG(materials_buffer_ != nullptr, "buffer not initialized");
  // assign handles so material samplers, properties can be accessed in shaders
  BindlessMaterial bindless_mat;
  if (material.albedo_texture != nullptr) {
    bindless_mat.albedo_map_handle = material.albedo_texture->BindlessHandle();
  }
  if (material.normal_texture != nullptr)
    bindless_mat.normal_map_handle = material.normal_texture->BindlessHandle();
  if (material.metalness_texture != nullptr)
    bindless_mat.metalness_map_handle = material.metalness_texture->BindlessHandle();
  if (material.ao_texture != nullptr)
    bindless_mat.ao_map_handle = material.ao_texture->BindlessHandle();
  if (material.roughness_texture != nullptr)
    bindless_mat.roughness_map_handle = material.roughness_texture->BindlessHandle();
  bindless_mat.albedo = material.albedo;
  bindless_mat.metallic_roughness = material.metallic_roughness;
  // id is the index into the material buffer.
  AssetHandle handle = materials_buffer_->SubData(sizeof(BindlessMaterial), &bindless_mat);

  // auto* ptr = static_cast<BindlessMaterial*>(materials_buffer_->Map(GL_READ_ONLY));
  // for (int i = 0; i < materials_buffer_->NumAllocs(); i++) {
  //   spdlog::info("handle: {}", ptr->albedo_map_handle);
  //   ptr++;
  // }
  // spdlog::warn("done");
  // materials_buffer_->Unmap();
  return handle;
}

}  // namespace engine::gfx
