#include "graphics_system.h"

#include "../../renderer/renderer.h"

void GraphicsSystem::Init() {}
void GraphicsSystem::Shutdown() {}
void GraphicsSystem::StartFrame() { gfx::renderer::StartFrame(); }
void GraphicsSystem::EndFrame() { gfx::renderer::EndFrame(); }
