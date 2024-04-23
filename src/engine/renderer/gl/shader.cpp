#include "shader.h"

#include <GL/glew.h>

#include <glm/gtc/type_ptr.hpp>

#include "../../e_assert.h"

void Shader::Bind() const {
  DEBUG_ASSERT_MSG(id_ != 0, "Shader is invalid");
  glUseProgram(id_);
}

void Shader::Unbind() { glUseProgram(0); }
