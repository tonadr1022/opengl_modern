#include "graphics_system.h"

#include "../../renderer/renderer.h"
#include "../../window.h"

void GraphicsSystem::Init() {
  window_.Init();
  window_.SetVsync(true);
}

void GraphicsSystem::Shutdown() {}
void GraphicsSystem::StartFrame() { renderer::StartFrame(); }

void GraphicsSystem::EndFrame() { window_.SwapBuffers(); }
