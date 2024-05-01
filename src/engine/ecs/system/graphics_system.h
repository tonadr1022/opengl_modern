#pragma once

class Scene;

struct GraphicsSystem {
 public:
  void Init();
  void Shutdown();
  void StartFrame(Scene& scene);
  void DrawOpaque(Scene& scene);
  void EndFrame();
};
