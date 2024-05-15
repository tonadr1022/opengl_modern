#pragma once

#include <cstdint>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <unordered_map>
#include <vector>

#include "engine/core/base.h"
#include "engine/renderer/light.h"
#include "engine/resource/data_types.h"
#include "engine/resource/paths.h"

struct GLFWwindow;

namespace engine {

class Engine;
}
namespace engine::gfx {

struct Buffer;
struct Texture2D;

struct RendererStats {
  uint32_t vertices_in_buffer{0};
  uint32_t indices_in_buffer{0};
  uint32_t multi_draw_calls{0};
  uint32_t multi_draw_cmds_buffer_count{0};
  uint32_t num_meshes{0};
  uint32_t num_point_lights{0};
};

struct UserDrawCommand {
  AssetHandle mesh_id;
  AssetHandle material_id;
  glm::mat4 model_matrix;
};

struct DrawElementsIndirectCommand {
  uint32_t count;
  uint32_t instance_count;
  uint32_t first_index;
  int base_vertex;
  uint32_t base_instance;
};

struct RenderViewInfo;
struct MaterialData;

class Renderer {
 public:
  static Renderer& Get();
  ~Renderer();
  void Init(glm::ivec2 framebuffer_dims);
  void Shutdown();
  void StartFrame(const RenderViewInfo& view_info);
  void EndFrame();
  void Reset();
  void ClearMeshes();
  void SetBatchedObjectCount(uint32_t count);
  void SubmitDynamicLights(std::vector<PointLight>& lights);
  void OnFrameBufferResize(uint32_t width, uint32_t height);
  void SubmitDrawCommand(const glm::mat4& model, AssetHandle mesh_handle,
                         AssetHandle material_handle);
  [[nodiscard]] AssetHandle AddBatchedMesh(std::vector<Vertex>& vertices,
                                           std::vector<Index>& indices);
  void RenderOpaqueObjects();
  void SetMaterials(const std::vector<MaterialData>& materials);
  [[nodiscard]] AssetHandle AddMaterial(const MaterialData& material_data);
  [[nodiscard]] const RendererStats& GetStats();

  static constexpr const uint32_t kMaxMaterials = 100;
  static constexpr uint32_t kVertexBufferArrayMaxLength{10'000'0000};
  static constexpr uint32_t kIndexBufferArrayMaxLength{10'000'0000};
  static constexpr uint32_t kMaxDrawCommands{10'000'000};
  static constexpr uint32_t kMaxLights{100};

  void OnImGuiRender();

 private:
  struct alignas(16) UBOUniforms {
    glm::mat4 vp_matrix;
    glm::vec3 cam_pos;
    float pad;
  };
  std::vector<std::string> skybox_strings_ = {
      GET_TEXTURE_PATH("skybox2/right.jpg"), GET_TEXTURE_PATH("skybox2/left.jpg"),
      GET_TEXTURE_PATH("skybox2/top.jpg"),   GET_TEXTURE_PATH("skybox2/bottom.jpg"),
      GET_TEXTURE_PATH("skybox2/front.jpg"), GET_TEXTURE_PATH("skybox2/back.jpg"),
  };
  friend class engine::Engine;
  Renderer();
  static Renderer* instance_;

  std::unique_ptr<Buffer> materials_buffer_{nullptr};
  std::unordered_map<AssetHandle, int> material_id_to_index_;

  std::unique_ptr<Buffer> batch_vertex_buffer_{nullptr};
  std::unique_ptr<Buffer> batch_element_buffer_{nullptr};
  std::unique_ptr<Buffer> batch_uniform_ssbo_{nullptr};
  std::unique_ptr<Buffer> light_ssbo_{nullptr};
  std::unique_ptr<Buffer> draw_indirect_buffer_{nullptr};
  std::unique_ptr<Buffer> shader_uniform_ubo_{nullptr};
  // uint32_t materials_buffer_;
  // std::unique_ptr<VertexArray> batch_vao_{nullptr};
  uint32_t batch_vao_{0};
  uint32_t g_buffer_{0};
  //  gfx::Texture2D* g_position_tex_{nullptr};
  // gfx::Texture2D* g_normal_tex_{nullptr};
  // gfx::Texture2D* g_albedo_tex_{nullptr};

  uint32_t g_albedo_tex_{0};
  uint32_t g_normal_tex_{0};
  uint32_t g_rma_tex_{0};
  uint32_t g_depth_tex_{0};

  void InitBuffers();
  void InitVaos();
  void LoadShaders();
  void InitFrameBuffers();
  void ResetFrameBuffers();

  // scene data
  glm::mat4 view_matrix_;
  glm::mat4 projection_matrix_;
  glm::mat4 vp_matrix_;

  std::vector<AssetHandle> draw_cmd_mesh_ids_;

  struct alignas(16) BatchUniform {
    glm::mat4 model_matrix;
    glm::mat4 normal_matrix;
    uint64_t material_index;
  };

  std::vector<BatchUniform> draw_cmd_uniforms_;

  // std::map<MeshID, DrawElementsIndirectCommand> mesh_buffer_info_;
  std::vector<DrawElementsIndirectCommand> draw_elements_indirect_cmds_;
  glm::ivec2 framebuffer_dims_{1, 1};

  void* batch_map_ptr_{nullptr};

  void DrawOpaqueHelper(AssetHandle material_id, std::vector<glm::mat4>& uniforms);
  RendererStats stats_{0};
  DirectionalLight dir_light_;
  bool dir_light_on_{false};
  bool normal_map_on_{true};
  bool roughness_map_on_{true};
  bool metallic_map_on_{true};

  struct InternalSettings {
    glm::vec3 albedo_override{1, 0, 0};
    float metallic_override{0};
    float roughness_override{1};
    bool override_material{false};
  };

  InternalSettings internal_settings_;
};

}  // namespace engine::gfx
