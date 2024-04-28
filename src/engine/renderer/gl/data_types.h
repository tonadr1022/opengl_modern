#pragma once

#include "engine/pch.h"

namespace gfx {

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec3 tangents;
  glm::vec2 tex_coords;
};

using Index = uint32_t;
// static const char* glGetTypeString(GLenum type) {
//   switch (type) {
//     case GL_BYTE:
//       return "GLbyte";
//     case GL_UNSIGNED_BYTE:
//       return "GLubyte";
//     case GL_SHORT:
//       return "GLshort";
//     case GL_UNSIGNED_SHORT:
//       return "GLushort";
//     case GL_INT:
//       return "GLint";
//     case GL_UNSIGNED_INT:
//       return "GLuint";
//     case GL_FLOAT:
//       return "GLfloat";
//     case GL_2_BYTES:
//       return "GL_2_BYTES";
//     case GL_3_BYTES:
//       return "GL_3_BYTES";
//     case GL_4_BYTES:
//       return "GL_4_BYTES";
//     case GL_DOUBLE:
//       return "GLdouble";
//     case GL_BUFFER:
//       return "GL_BUFFER";
//     case GL_SHADER:
//       return "GL_SHADER";
//     case GL_PROGRAM:
//       return "GL_PROGRAM";
//     case GL_VERTEX_ARRAY:
//       return "GL_VERTEX_ARRAY";
//
//     default:
//       return "not a GLenum type";
//   }
}  // namespace gfx
