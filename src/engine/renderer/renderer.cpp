#include "renderer.h"

#include <GL/glew.h>
#include <spdlog/spdlog.h>

namespace gfx {

namespace renderer {
void StartFrame() {
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_STENCIL_TEST);
  glClearColor(0.0, 2.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}
}  // namespace renderer

}  // namespace gfx
