#pragma once

#include "engine/renderer/data_types.h"

struct GLFWwindow;

namespace gfx {

struct RendererStats {
  uint32_t vertices;
  uint32_t indices;
  uint32_t multi_draw_calls;
  uint32_t meshes_drawn;
  uint32_t material_swaps;
  uint32_t shader_swaps;
  uint32_t meshes_in_memory;
};

class Renderer {
 public:
  static void Init();
  static void StartFrame(const glm::mat4& view_matrix, const glm::mat4& projection_matrix);
  static void EndFrame();
  static void Restart();
  static void SetBatchedObjectCount(uint32_t count);
  static void SetFrameBufferSize(uint32_t width, uint32_t height);
  static void SubmitDrawCommand(const glm::mat4& model, MeshID mesh_id, MaterialID material_id);
  static void AddBatchedMesh(MeshID id, std::vector<Vertex>& vertices, std::vector<Index>& indices);
  static void AddMesh(MeshID id, int vertices, int indices);
  static void RenderOpaqueObjects();
  static const RendererStats& GetStats();

 private:
};

}  // namespace gfx
