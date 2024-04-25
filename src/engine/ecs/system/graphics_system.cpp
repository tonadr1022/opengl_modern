#include "graphics_system.h"

#include "../../application/window.h"
#include "../../renderer/renderer.h"

void GraphicsSystem::Init() {
  window_.Init();
  window_.SetVsync(true);
}

void GraphicsSystem::Shutdown() { window_.Shutdown(); }
void GraphicsSystem::StartFrame() { gfx::renderer::StartFrame(); }

void GraphicsSystem::EndFrame() { window_.SwapBuffers(); }
