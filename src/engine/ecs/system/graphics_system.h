#pragma once

class Scene;

struct GraphicsSystem {
 public:
  void Init();
  void Shutdown();
  void StartFrame();
  void DrawOpaque(Scene &scene);
  void EndFrame();
};
