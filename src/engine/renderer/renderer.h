#pragma once

struct GLFWwindow;
struct Vertex;
struct Index;
struct MeshID;

namespace gfx {
namespace renderer {

extern void Init();
extern void StartFrame();
extern void EndFrame();
extern void ClearAllData();
extern void AddBatchedMesh(MeshID id, std::vector<Vertex>& vertices, std::vector<Index>& indices);

}  // namespace renderer

}  // namespace gfx
