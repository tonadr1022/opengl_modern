#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <map>
#include <memory>
#include <vector>

#include "engine/renderer/gl/vertex_array.h"
#include "engine/resource/data_types.h"
#include "gl/buffer.h"

struct GLFWwindow;

namespace engine {
class ShaderManager;
}
namespace engine::gfx {

struct RendererStats {
  uint32_t vertices;
  uint32_t indices;
  uint32_t multi_draw_calls;
  uint32_t meshes_drawn;
  uint32_t material_swaps;
  uint32_t shader_swaps;
  uint32_t meshes_in_memory;
};

struct UserDrawCommand {
  MeshID mesh_id;
  MaterialID material_id;
  uint32_t model_matrix_index;
};

struct DrawElementsIndirectCommand {
  uint32_t count;
  uint32_t instance_count;
  uint32_t first_index;
  int base_vertex;
  uint32_t base_instance;
};

struct RenderViewInfo;

class Renderer {
 public:
  explicit Renderer(ShaderManager& shader_manager);
  void Init();
  void Shutdown();
  void StartFrame(const RenderViewInfo& camera_matrices);
  void EndFrame();
  void Restart();
  void SetBatchedObjectCount(uint32_t count);
  void SetStaticObjectCount(uint32_t count);
  void SetFrameBufferSize(uint32_t width, uint32_t height);
  void SubmitDrawCommand(const glm::mat4& model, MeshID mesh_id, MaterialID material_id);
  void AddBatchedMesh(MeshID id, std::vector<Vertex>& vertices, std::vector<Index>& indices);
  void RenderOpaqueObjects();
  const RendererStats& GetStats();

 private:
  ShaderManager& shader_manager_;
  std::unique_ptr<Buffer> materials_buffer_{nullptr};
  std::unique_ptr<Buffer> batch_vertex_buffer_{nullptr};
  std::unique_ptr<Buffer> batch_element_buffer_{nullptr};
  std::unique_ptr<Buffer> batch_ssbo_buffer_{nullptr};
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

  std::vector<UserDrawCommand> user_draw_cmds_;
  std::vector<glm::mat4> user_draw_cmd_model_matrices_;
  uint32_t user_draw_cmds_index_{0};

  std::map<MeshID, DrawElementsIndirectCommand> mesh_buffer_info_;
  uint32_t framebuffer_width_{1}, frame_buffer_height_{1};

  std::vector<glm::mat4> uniforms_;
  void* batch_map_ptr_{nullptr};

  void DrawOpaqueHelper(MaterialID material_id, std::vector<glm::mat4>& uniforms);
  RendererStats stats_{0};
};

}  // namespace engine::gfx
