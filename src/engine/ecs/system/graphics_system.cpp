#include "graphics_system.h"

#include "../../renderer/renderer.h"

void GraphicsSystem::Init() { window_ = renderer::InitContext(); }
