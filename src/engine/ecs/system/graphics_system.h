#pragma once

struct GraphicsSystem {
 public:
  void Init();
  void Shutdown();
  void StartFrame();
  void EndFrame();
};
