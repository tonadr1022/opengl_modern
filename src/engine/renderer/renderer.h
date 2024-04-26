#pragma once

struct GLFWwindow;

namespace gfx {
namespace renderer {

extern void StartFrame();
extern void EndFrame();
extern void ClearAllData();
extern void AddBatchedMesh();

}  // namespace renderer

}  // namespace gfx
