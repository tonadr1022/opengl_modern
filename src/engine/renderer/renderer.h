#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

#include "engine/renderer/gl/vertex_array.h"
#include "engine/resource/data_types.h"
#include "gl/buffer.h"

struct GLFWwindow;

namespace engine {
class ShaderManager;
}  // namespace engine

namespace engine::gfx {

struct RendererStats {
  uint32_t vertices;
  uint32_t indices;
  uint32_t multi_draw_calls;
  uint32_t meshes_drawn;
  uint32_t material_swaps;
  uint32_t shader_swaps;
  uint32_t num_meshes;
};

struct UserDrawCommand {
  MeshID mesh_id;
  MaterialID material_id;
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
  explicit Renderer(ShaderManager& shader_manager);
  void Init();
  void Shutdown();
  void StartFrame(const RenderViewInfo& camera_matrices);
  void EndFrame();
  void Restart();
  void SetBatchedObjectCount(uint32_t count);
  void SetFrameBufferSize(uint32_t width, uint32_t height);
  void SubmitDrawCommand(const glm::mat4& model, MeshID mesh_id, MaterialID material_id);
  [[nodiscard]] MeshID AddBatchedMesh(std::vector<Vertex>& vertices, std::vector<Index>& indices);
  // [[nodiscard]] MeshID AddBatchedMesh(MeshID id, std::vector<Vertex>& vertices,
  //                                     std::vector<Index>& indices);
  void RenderOpaqueObjects();
  void SetMaterials(const std::vector<MaterialData>& materials);
  [[nodiscard]] MaterialID AddMaterial(const MaterialData& material_data);
  [[nodiscard]] const RendererStats& GetStats();

  static constexpr const uint32_t MaxMaterials = 512;

 private:
  ShaderManager& shader_manager_;

  std::unique_ptr<Buffer> materials_buffer_{nullptr};
  std::unordered_map<MaterialID, int> material_id_to_index_;

  std::unique_ptr<Buffer> batch_vertex_buffer_{nullptr};
  std::unique_ptr<Buffer> batch_element_buffer_{nullptr};
  std::unique_ptr<Buffer> batch_ssbo_uniform_buffer_{nullptr};
  std::unique_ptr<Buffer> draw_indirect_buffer_{nullptr};
  // std::unique_ptr<VertexArray> batch_vao_{nullptr};
  uint32_t batch_vao_{0};
  void InitBuffers();
  void InitVaos();
  void LoadShaders();

  // scene data
  glm::mat4 view_matrix_;
  glm::mat4 projection_matrix_;
  glm::mat4 vp_matrix_;

  std::vector<MeshID> draw_cmd_mesh_ids_;

  struct BatchUniform {
    glm::mat4 model_matrix;
    uint64_t material_index;
  };
  std::vector<BatchUniform> draw_cmd_uniforms_;

  // std::map<MeshID, DrawElementsIndirectCommand> mesh_buffer_info_;
  std::vector<DrawElementsIndirectCommand> mesh_buffer_info_;
  uint32_t framebuffer_width_{1}, frame_buffer_height_{1};

  void* batch_map_ptr_{nullptr};

  void DrawOpaqueHelper(MaterialID material_id, std::vector<glm::mat4>& uniforms);
  RendererStats stats_{0};
};

}  // namespace engine::gfx
