#pragma once

#include "../../application/window.h"

class GraphicsSystem {
 public:
  void Init();
  void Shutdown();
  void StartFrame();
  void EndFrame();
  [[nodiscard]] const Window& GetWindow() const { return window_; }

 private:
  Window window_;
};
