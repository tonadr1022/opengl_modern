#pragma once

#include "engine/renderer/resource/material_manager.h"
#include "engine/renderer/resource/mesh_manager.h"

struct GLFWwindow;
struct Vertex;
struct Index;
struct MaterialID;

namespace gfx {
namespace renderer {

extern void Init();
extern void StartFrame();
extern void EndFrame();
extern void Restart();
extern void SetBatchedObjectCount(uint32_t count);
extern void SubmitDrawCommand(const glm::mat4& model, MeshID mesh_id, MaterialID material_id);
extern void AddBatchedMesh(MeshID id, std::vector<Vertex>& vertices, std::vector<Index>& indices);
extern void RenderOpaqueObjects();

}  // namespace renderer

}  // namespace gfx
