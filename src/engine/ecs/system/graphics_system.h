#pragma once

#include "../../window.h"

class GraphicsSystem {
 public:
  void Init();
  void Shutdown();
  void StartFrame();
  void EndFrame();

 private:
  Window window_;
};
