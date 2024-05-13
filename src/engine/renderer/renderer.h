#pragma once

#include <cstdint>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <unordered_map>
#include <vector>

#include "engine/core/base.h"
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
  uint32_t vertices;
  uint32_t indices;
  uint32_t multi_draw_calls;
  uint32_t multi_draw_cmds_buffer_count;
  uint32_t num_meshes;
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
  void SetBatchedObjectCount(uint32_t count);
  void OnFrameBufferResize(uint32_t width, uint32_t height);
  void SubmitDrawCommand(const glm::mat4& model, AssetHandle mesh_handle,
                         AssetHandle material_handle);
  [[nodiscard]] AssetHandle AddBatchedMesh(std::vector<Vertex>& vertices,
                                           std::vector<Index>& indices);
  void RenderOpaqueObjects();
  void SetMaterials(const std::vector<MaterialData>& materials);
  [[nodiscard]] AssetHandle AddMaterial(const MaterialData& material_data);
  [[nodiscard]] const RendererStats& GetStats();

  static constexpr const uint32_t MaxMaterials = 100;

 private:
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
  std::unique_ptr<Buffer> batch_ssbo_uniform_buffer_{nullptr};
  std::unique_ptr<Buffer> draw_indirect_buffer_{nullptr};
  std::unique_ptr<Buffer> shader_uniform_ubo_{nullptr};
  // uint32_t materials_buffer_;
  // std::unique_ptr<VertexArray> batch_vao_{nullptr};
  uint32_t batch_vao_{0};
  uint32_t g_buffer_{0};
  //  gfx::Texture2D* g_position_tex_{nullptr};
  // gfx::Texture2D* g_normal_tex_{nullptr};
  // gfx::Texture2D* g_albedo_tex_{nullptr};

  uint32_t g_position_tex_{0};
  uint32_t g_normal_tex_{0};
  uint32_t g_albedo_tex_{0};

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
    glm::mat4 model_matrix{};
    uint64_t material_index{};
  };

  std::vector<BatchUniform> draw_cmd_uniforms_;

  // std::map<MeshID, DrawElementsIndirectCommand> mesh_buffer_info_;
  std::vector<DrawElementsIndirectCommand> draw_elements_indirect_cmds_;
  glm::ivec2 framebuffer_dims_{1, 1};

  void* batch_map_ptr_{nullptr};

  void DrawOpaqueHelper(AssetHandle material_id, std::vector<glm::mat4>& uniforms);
  RendererStats stats_{0};
};

}  // namespace engine::gfx
