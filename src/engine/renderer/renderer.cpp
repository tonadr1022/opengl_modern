#include "renderer.h"

#include <imgui.h>

#include <cstdint>
#include <glm/gtc/type_ptr.hpp>

#include "engine/core/base.h"
#include "engine/core/e_assert.h"
#include "engine/pch.h"
#include "engine/renderer/gl/buffer.h"
#include "engine/renderer/gl/debug.h"
#include "engine/renderer/gl/texture_2d.h"
#include "engine/renderer/light.h"
#include "engine/renderer/material.h"
#include "engine/resource/paths.h"
#include "engine/resource/shader_manager.h"
#include "renderer_types.h"

namespace engine::gfx {

namespace {
struct CameraDirection {
  GLenum cube_map_face;
  glm::vec3 target;
  glm::vec3 up;
};
constexpr CameraDirection kCameraDirections[6] = {
    {GL_TEXTURE_CUBE_MAP_POSITIVE_X, {1, 0, 0}, {0, 1, 0}},
    {GL_TEXTURE_CUBE_MAP_NEGATIVE_X, {-1, 0, 0}, {0, 1, 0}},
    {GL_TEXTURE_CUBE_MAP_POSITIVE_Y, {0, 1, 0}, {0, 0, -1}},
    {GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, {0, -1, 0}, {0, 0, 1}},
    {GL_TEXTURE_CUBE_MAP_POSITIVE_Z, {0, 0, 1}, {0, 1, 0}},
    {GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, {0, 0, -1}, {0, 1, 0}}};
}  // namespace

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

  manager.AddShader("depth", {{GET_SHADER_PATH("depth.vs.glsl"), ShaderType::Vertex},
                              {GET_SHADER_PATH("depth.fs.glsl"), ShaderType::Fragment}});
  manager.AddShader("point_depth",
                    {{GET_SHADER_PATH("point_depth.vs.glsl"), ShaderType::Vertex},
                     {GET_SHADER_PATH("point_depth.fs.glsl"), ShaderType::Fragment}});
  manager.AddShader("depthquad", {{GET_SHADER_PATH("depthquad.vs.glsl"), ShaderType::Vertex},
                                  {GET_SHADER_PATH("depthquad.fs.glsl"), ShaderType::Fragment}});
}

Renderer::~Renderer() = default;

void Renderer::InitVaos() {
  // batch
  glCreateVertexArrays(1, &batch_vao_);
  glEnableVertexArrayAttrib(batch_vao_, 0);
  glEnableVertexArrayAttrib(batch_vao_, 1);
  glEnableVertexArrayAttrib(batch_vao_, 2);
  glEnableVertexArrayAttrib(batch_vao_, 3);
  glVertexArrayAttribFormat(batch_vao_, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
  glVertexArrayAttribFormat(batch_vao_, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
  glVertexArrayAttribFormat(batch_vao_, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, tex_coords));
  glVertexArrayAttribFormat(batch_vao_, 3, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, tangent));
  glVertexArrayAttribBinding(batch_vao_, 0, 0);
  glVertexArrayAttribBinding(batch_vao_, 1, 0);
  glVertexArrayAttribBinding(batch_vao_, 2, 0);
  glVertexArrayAttribBinding(batch_vao_, 3, 0);

  // quad
  glCreateVertexArrays(1, &quad_.vao);
  glEnableVertexArrayAttrib(quad_.vao, 0);
  glEnableVertexArrayAttrib(quad_.vao, 1);
  glVertexArrayAttribFormat(quad_.vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(VertexQuad, position));
  glVertexArrayAttribFormat(quad_.vao, 1, 2, GL_FLOAT, GL_FALSE, offsetof(VertexQuad, tex_coords));
  glVertexArrayAttribBinding(quad_.vao, 0, 0);
  glVertexArrayAttribBinding(quad_.vao, 1, 0);
}

void Renderer::InitBuffers() {
  // GL_DYNAMIC_STORAGE_BIT - contents can be update after creation
  batch_vertex_buffer_ = std::make_unique<Buffer>(sizeof(Vertex) * kVertexBufferArrayMaxLength,
                                                  GL_DYNAMIC_STORAGE_BIT);
  batch_element_buffer_ =
      std::make_unique<Buffer>(sizeof(Index) * kIndexBufferArrayMaxLength, GL_DYNAMIC_STORAGE_BIT);
  glVertexArrayVertexBuffer(batch_vao_, 0, batch_vertex_buffer_->Id(), 0, sizeof(Vertex));
  glVertexArrayElementBuffer(batch_vao_, batch_element_buffer_->Id());

  // stores draw indirect commands
  draw_indirect_buffer_ = std::make_unique<Buffer>(
      sizeof(DrawElementsIndirectCommand) * kMaxDrawCommands, GL_DYNAMIC_STORAGE_BIT);
  // stores uniform data: model matrix, normal matrix, material index
  batch_uniform_ssbo_ =
      std::make_unique<Buffer>(sizeof(BatchUniform) * kMaxDrawCommands, GL_DYNAMIC_STORAGE_BIT);
  // stores bindless handles and pbr data
  materials_buffer_ = std::make_unique<Buffer>(sizeof(BindlessMaterial) * kMaxMaterials,
                                               GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT);

  // stores camera vp_matrix and camera position
  shader_uniform_ubo_ = std::make_unique<Buffer>(sizeof(UBOUniforms), GL_DYNAMIC_STORAGE_BIT);
  // stores all point lights
  light_ssbo_ = std::make_unique<Buffer>(sizeof(PointLight) * kMaxLights, GL_DYNAMIC_STORAGE_BIT);

  // quad vbo
  quad_.buffer = std::make_unique<Buffer>(sizeof(VertexQuad) * quad_.kVertices.size(), 0,
                                          (void*)quad_.kVertices.data());
  glVertexArrayVertexBuffer(quad_.vao, 0, quad_.buffer->Id(), 0, sizeof(VertexQuad));

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
  /*ResetDepthMapFBO();*/
}

void Renderer::SetBatchedObjectCount(uint32_t count) {
  draw_cmd_mesh_ids_.reserve(count);
  draw_cmd_uniforms_.reserve(count);
}

void Renderer::SubmitDrawCommand(const glm::mat4& model, AssetHandle mesh_handle,
                                 AssetHandle material_handle) {
  ZoneScopedN("submit draw");
  draw_cmd_mesh_ids_.emplace_back(mesh_handle);
  glm::mat3 normal_matrix = glm::transpose(glm::inverse(glm::mat3(model)));
  draw_cmd_uniforms_.emplace_back(model, normal_matrix, material_handle);
}

const RendererStats& Renderer::GetStats() { return stats_; }

void Renderer::OnImGuiRender() {
  ImGui::Begin("Renderer", nullptr, ImGuiWindowFlags_NoNavFocus);
  // if (ImGui::TreeNodeEx("Stats", ImGuiTreeNodeFlags_DefaultOpen)) {
  if (ImGui::CollapsingHeader("Metrics", ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::Text("Vertices in buffer: %i", stats_.vertices_in_buffer);
    ImGui::Text("Indices in buffer: %i", stats_.indices_in_buffer);
    ImGui::Text("MultiDrawCalls: %i", stats_.multi_draw_calls);
    ImGui::Text("Multidraw commands Buffer: %i", stats_.multi_draw_cmds_buffer_count);
    ImGui::Text("Num Meshes: %i", stats_.num_meshes);
    ImGui::Text("Num Point Lights: %i", stats_.num_point_lights);
    ImGui::Text("VBO offset: %i, %f", batch_vertex_buffer_->Offset(),
                static_cast<float>(batch_vertex_buffer_->Offset()) / kVertexBufferArrayMaxLength);
    ImGui::Text("EBO Offset: %i, %f", batch_element_buffer_->Offset(),
                static_cast<float>(batch_element_buffer_->Offset()) / kIndexBufferArrayMaxLength);
  }

  if (ImGui::CollapsingHeader("PBR", ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::Checkbox("Override Material", &settings_.override_material);
    ImGui::ColorEdit3("Albedo Override", &settings_.albedo_override.x);
    ImGui::DragFloat("Metallic Override", &settings_.metallic_override, .01, 0, 1);
    ImGui::DragFloat("Roughness Override", &settings_.roughness_override, .01, 0, 1);
  }
  if (ImGui::CollapsingHeader("Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::Checkbox("normal map", &normal_map_on_);
    ImGui::Checkbox("front cull shadow", &front_cull_shadow_);
    ImGui::Checkbox("roughness map", &roughness_map_on_);
    ImGui::Checkbox("metallic map", &metallic_map_on_);
  }
  ImGui::End();
}

void Renderer::TestGBufferReset() {
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

void Renderer::ResetDepthMapFBO() {
  /////////////////////////////// Depth texture ///////////////////////////////////////
  if (depth_tex_) glDeleteTextures(1, &depth_tex_);
  glCreateTextures(GL_TEXTURE_2D, 1, &depth_tex_);
  glTextureStorage2D(depth_tex_, 1, GL_DEPTH_COMPONENT32F, kShadowMapDims.x, kShadowMapDims.y);
  glTextureParameteri(depth_tex_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTextureParameteri(depth_tex_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  // coordinated outside of depth map range are not in shadow, so should have value of one, solved
  // by clamp to border
  glTextureParameteri(depth_tex_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTextureParameteri(depth_tex_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  constexpr const float kBorderColor[] = {1, 1, 1, 1};
  glNamedFramebufferTexture(depth_map_fbo_, GL_DEPTH_ATTACHMENT, depth_tex_, 0);
  glTextureParameterfv(depth_tex_, GL_TEXTURE_BORDER_COLOR, kBorderColor);

  // frame buffer is not complete without color, so must specify so.
  glNamedFramebufferDrawBuffer(depth_map_fbo_, GL_NONE);
  glNamedFramebufferReadBuffer(depth_map_fbo_, GL_NONE);
  if (!glCheckNamedFramebufferStatus(depth_map_fbo_, GL_FRAMEBUFFER)) {
    spdlog::error("depth map fbo incomplete.");
  }
}

void Renderer::InitFrameBuffers() {
  glCreateFramebuffers(1, &depth_map_fbo_);
  // glCreateFramebuffers(1, &g_buffer_);
  ResetDepthMapFBO();
}

void Renderer::Init(glm::ivec2 framebuffer_dims) {
  framebuffer_dims_ = framebuffer_dims;
  glViewport(0, 0, framebuffer_dims_.x, framebuffer_dims_.y);
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(MessageCallback, nullptr);
  LoadShaders();
  InitVaos();
  InitBuffers();
  InitFrameBuffers();

  // point light depth buffer attachment
  glCreateTextures(GL_TEXTURE_2D, 1, &point_depth_buffer_tex_);
  glTextureStorage2D(point_depth_buffer_tex_, 1, GL_DEPTH_COMPONENT32F, 1024, 1024);
  glTextureParameteri(point_depth_buffer_tex_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTextureParameteri(point_depth_buffer_tex_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTextureParameteri(point_depth_buffer_tex_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTextureParameteri(point_depth_buffer_tex_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // point light depth FBO
  glCreateFramebuffers(1, &point_depth_fbo_);
  glNamedFramebufferTexture(point_depth_fbo_, GL_DEPTH_ATTACHMENT, point_depth_buffer_tex_, 0);
  // no read/write to draw buffer
  glNamedFramebufferDrawBuffer(point_depth_fbo_, GL_NONE);
  glNamedFramebufferReadBuffer(point_depth_fbo_, GL_NONE);
  if (!glCheckNamedFramebufferStatus(point_depth_fbo_, GL_FRAMEBUFFER)) {
    spdlog::error("point light depth map fbo incomplete.");
  }
  // point cube map texture
  glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &point_depth_cube_map_tex_);
  const glm::ivec2 shadow_cube_map_dims{1024, 1024};
  glTextureStorage2D(point_depth_cube_map_tex_, 1, GL_R32F, shadow_cube_map_dims.x,
                     shadow_cube_map_dims.y);
  // for (unsigned int i = 0; i < 6; ++i) {
  //   glTextureSubImage3D(point_depth_cube_map_tex_, 0, 0, 0, i, shadow_cube_map_dims.x,
  //                       shadow_cube_map_dims.y, 0, GL_RED, GL_FLOAT, nullptr);
  // }
  /*
   *GL_CLAMP_TO_EDGE: clamps texture coordinates at all mipmap levels such that the texture filter
   *never samples a border texel. The color returned when clamping is derived only from texels at
   *the edge of the texture image
   */
  glTextureParameteri(point_depth_cube_map_tex_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTextureParameteri(point_depth_cube_map_tex_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTextureParameteri(point_depth_cube_map_tex_, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTextureParameteri(point_depth_cube_map_tex_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTextureParameteri(point_depth_cube_map_tex_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Renderer::Shutdown() {}

void Renderer::StartFrame(const RenderViewInfo& view_info) {
  // glEnable(GL_FRAMEBUFFER_SRGB);
  // can't memset since num meshes is across frames
  stats_.multi_draw_calls = 0;
  stats_.multi_draw_cmds_buffer_count = 0;
  stats_.num_point_lights = 0;

  // shader matrix uniforms
  view_matrix_ = view_info.view_matrix;
  projection_matrix_ = view_info.projection_matrix;
  vp_matrix_ = projection_matrix_ * view_matrix_;

  // assign uniforms common to shaders
  shader_uniform_ubo_->BindBase(GL_UNIFORM_BUFFER, 0);
  shader_uniform_ubo_->ResetOffset();
  shader_uniform_ubo_->SubData(sizeof(glm::mat4), glm::value_ptr(vp_matrix_));
  // spdlog::info("cam pos {} {} {}", view_info.cam_pos.x, view_info.cam_pos.y,
  // view_info.cam_pos.z);

  dir_light_on_ = view_info.dir_light_on;
  glm::vec3 cam_pos = view_info.cam_pos;
  shader_uniform_ubo_->SubData(sizeof(glm::vec4), &cam_pos.x);

  draw_cmd_uniforms_.clear();
  draw_cmd_mesh_ids_.clear();

  // gl state
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_STENCIL_TEST);

  // glClearColor(0.6, 0.6, 0.6, 1.0);
  glClearColor(0.0, 0.0, 0.0, 1.0);
  // TODO(tony): address when blitting framebuffers
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
  stats_.indices_in_buffer += cmd.count;
  stats_.vertices_in_buffer += vertices.size();
  return id;
}

void Renderer::RenderOpaqueObjects(const RenderViewInfo& view_info,
                                   const DirectionalLight& dir_light) {
  {
    ZoneScopedN("Setup draw commands");
    glBindVertexArray(batch_vao_);
    batch_uniform_ssbo_->ResetOffset();
    batch_uniform_ssbo_->SubData(sizeof(BatchUniform) * draw_cmd_uniforms_.size(),
                                 draw_cmd_uniforms_.data());
    per_frame_cmds_.clear();
    per_frame_cmds_.reserve(draw_cmd_uniforms_.size());
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
      stats_.multi_draw_cmds_buffer_count++;
      per_frame_cmds_.emplace_back(cmd);
    }

    draw_indirect_buffer_->ResetOffset();
    draw_indirect_buffer_->SubData(sizeof(DrawElementsIndirectCommand) * per_frame_cmds_.size(),
                                   per_frame_cmds_.data());
    // std::memcpy(batch_map_ptr, uniforms.data(), sizeof(glm::mat4) * uniforms.size());
    draw_indirect_buffer_->Bind(GL_DRAW_INDIRECT_BUFFER);
    batch_uniform_ssbo_->BindBase(GL_SHADER_STORAGE_BUFFER, 0);
  }

  glm::vec3 light_world_pos{0, 1, 0};
  glm::mat4 light_space_matrix;
  {
    ZoneScopedN("Shadow pass");
    glViewport(0, 0, kShadowMapDims.x, kShadowMapDims.y);
    glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo_);
    // only depth is written to so only clear depth
    glClear(GL_DEPTH_BUFFER_BIT);
    //////////////////////////////// Temp Settings///////////////////////////////////////////
    static float near_plane = 0.1;
    static float far_plane = 7.5;
    static float length{10};
    static float light_pos_mult{1.0f};

    // slope scale depth bias. Increases bias written to depth buffer based on delta z
    // static bool polygonoffset{false};
    // static glm::vec2 polygonoffsetfactors{1, 1};

    ImGui::Begin("Shadow map");
    // ImGui::DragFloat2("polygon offset", &polygonoffsetfactors.x, 0.1, 0, 10);
    // if (ImGui::Checkbox("Polygon offset", &polygonoffset)) {
    //   if (polygonoffset) {
    //     glEnable(GL_POLYGON_OFFSET_FILL);
    //   } else {
    //     glDisable(GL_POLYGON_OFFSET_FILL);
    //   }
    // }
    // glPolygonOffset(polygonoffsetfactors.x, polygonoffsetfactors.y);

    ImGui::DragFloat("near plane", &near_plane, 0.1, 0, 10);
    ImGui::DragFloat("far plane", &far_plane, 0.1, 1, 1000);
    ImGui::DragFloat("ortho length/2", &length, 0.1, 1, 1000);
    ImGui::DragFloat("light_pos multiplier", &light_pos_mult, 0.1, 0.5, 1000);
    /////////////////////////////////////////////////////////////////////////////////////

    // directional: all rays same direction, so use orthographic projection
    const glm::mat4 light_projection =
        glm::ortho(-length, length, -length, length, near_plane, far_plane);
    // light position is the magnitude multiplier * direction, farther away from origin
    glm::vec3 light_pos = -glm::normalize(dir_light.direction) * light_pos_mult;
    const glm::mat4 light_view =
        glm::lookAt(light_pos, light_pos + glm::normalize(dir_light.direction), glm::vec3(0, 1, 0));
    light_space_matrix = light_projection * light_view;
    auto shader = ShaderManager::Get().GetShader("depth");
    shader->Bind();
    shader->SetMat4("u_lightSpaceMatrix", light_space_matrix);

    glBindVertexArray(batch_vao_);

    // MAYBE TEMPORARY? front cull objects to prevent peter panning
    if (front_cull_shadow_) glCullFace(GL_FRONT);
    glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, per_frame_cmds_.size(), 0);
    if (front_cull_shadow_) glCullFace(GL_BACK);

    // TODO(tony): move
    ImGui::Image((void*)static_cast<intptr_t>(depth_tex_),
                 ImVec2(kShadowMapDims.x * .5, kShadowMapDims.y * .5), ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();

    ///////////////////////////////   Point light //////////////////////////////////////////////
    glClearColor(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);
    // TODO(tony): no magic number
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, point_depth_fbo_);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glViewport(0, 0, 1024, 1024);
    auto point_depth_shader = ShaderManager::Get().GetShader("point_depth");
    point_depth_shader->Bind();
    point_depth_shader->SetVec3("u_lightWorldPos", light_world_pos);
    // 90 degree projection matrix for each direction of the cube map.
    glm::mat4 proj_matrix = glm::perspective(90.f, 1.f, 0.1f, 100.f);
    // render to the cube map from each direction.
    for (int i = 0; i < 6; i++) {
      const auto& cam_dir_param = kCameraDirections[i];
      // using color buffer
      glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
      glm::mat4 view_matrix = glm::lookAt(light_world_pos, cam_dir_param.target, cam_dir_param.up);
      point_depth_shader->SetMat4("vp_matrix", proj_matrix * view_matrix);
      // glNamedFramebufferTexture(point_depth_fbo_, GL_COLOR_ATTACHMENT0,
      // point_depth_cube_map_tex_,
      //                           0);
      glNamedFramebufferTextureLayer(point_depth_fbo_, GL_COLOR_ATTACHMENT0,
                                     point_depth_cube_map_tex_, 0, i);
      glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, per_frame_cmds_.size(),
                                  0);
    }
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  }

  ///////////////////////////////////////////////  Final pass ////////////////////////
  {
    ZoneScopedN("Render Opaque Objects");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // set viewport since depth viewport was different
    glViewport(0, 0, framebuffer_dims_.x, framebuffer_dims_.y);
    // bind depth map texture that was written to in the shadow map pass
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depth_tex_);

    auto shader = ShaderManager::Get().GetShader("batch");
    shader->Bind();
    shader->SetBool("u_overrideMaterial", settings_.override_material);
    if (settings_.override_material) {
      shader->SetVec2("u_metallicRoughnessOverride",
                      {settings_.metallic_override, settings_.roughness_override});
      shader->SetVec3("u_albedoOverride", settings_.albedo_override);
    }
    shader->SetMat4("u_lightSpaceMatrix", light_space_matrix);
    shader->SetVec3("u_directionalColor", dir_light.color);
    shader->SetBool("u_directionalOn", dir_light_on_);
    shader->SetBool("u_normalMapOn", normal_map_on_);
    shader->SetBool("u_roughnessMapOn", roughness_map_on_);
    shader->SetBool("u_metallicMapOn", metallic_map_on_);
    shader->SetBool("u_normalMapOn", normal_map_on_);
    shader->SetVec3("u_directionalDirection", dir_light.direction);
    shader->SetVec3("u_pointLightShadowPos", light_world_pos);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, point_depth_cube_map_tex_);

    materials_buffer_->BindBase(GL_SHADER_STORAGE_BUFFER, 1);
    // TODO(tony): avoid subdata every frame?
    light_ssbo_->BindBase(GL_SHADER_STORAGE_BUFFER, 2);
    // mode, type, offest ptr, command count, stride (0 since tightly packed)
    glBindVertexArray(batch_vao_);
    glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, per_frame_cmds_.size(), 0);
    stats_.multi_draw_calls++;
  }
}

void Renderer::SubmitDynamicLights(std::vector<PointLight>& lights) {
  stats_.num_point_lights += lights.size();
  light_ssbo_->ResetOffset();
  light_ssbo_->SubData(sizeof(PointLight) * lights.size(), lights.data());
}

void Renderer::EndFrame() {
  // glDisable(GL_FRAMEBUFFER_SRGB);
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
  bindless_mat.albedo = material.albedo;
  bindless_mat.roughness = material.roughness;
  bindless_mat.metallic = material.metallic;

  // id is the index into the material buffer.
  AssetHandle handle = materials_buffer_->SubData(sizeof(BindlessMaterial), &bindless_mat);
  return handle;
}
}  // namespace engine::gfx
