#include "debug.h"

#include "engine/pch.h"
namespace gfx {

// adapted from https://learnopengl.com/In-Practice/Debugging

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                                GLsizei /*length*/, const GLchar* message,
                                const void* /*userParam*/) {
  // ignore insignificant error/warning codes
  if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;
  if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;

  const char* type_str = "";
  switch (type) {
    case GL_DEBUG_TYPE_ERROR:
      type_str = "Error";
      break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
      type_str = "Deprecated Behavior";
      break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
      type_str = "Undefined Behavior";
      break;
    case GL_DEBUG_TYPE_PORTABILITY:
      type_str = "Portability";
      break;
    case GL_DEBUG_TYPE_PERFORMANCE:
      type_str = "Performance";
      break;
    case GL_DEBUG_TYPE_MARKER:
      type_str = "Marker";
      break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
      type_str = "Push Group";
      break;
    case GL_DEBUG_TYPE_POP_GROUP:
      type_str = "Pop Group";
      break;
  }

  const char* severity_str = "";
  switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
      type_str = "High";
      break;
    case GL_DEBUG_SEVERITY_MEDIUM:
      type_str = "Medium";
      break;
    case GL_DEBUG_SEVERITY_LOW:
      type_str = "Low";
      break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
      type_str = "Notification";
      break;
  }
  const char* source_str = "";
  switch (source) {
    case GL_DEBUG_SOURCE_API:
      source_str = "API";
      break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
      source_str = "Window System";
      break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
      source_str = "Shader Compiler";
      break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
      source_str = "Third Party";
      break;
    case GL_DEBUG_SOURCE_APPLICATION:
      source_str = "Application";
      break;
    case GL_DEBUG_SOURCE_OTHER:
      source_str = "Other";
      break;
  }
  std::cerr << "Source: " << source_str << "  " << "Severity: " << severity_str
            << "  Type: " << type_str << ", Message: " << message << "\n";

  // spdlog::error("Source: {}, Id: {}, Type: {}, Severity: {}, Message: {}", source_str, id,
  // type_str,
  //               severity_str, message);
}

void GLAPIENTRY GLMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                                  [[maybe_unused]] GLsizei length, const GLchar* message,
                                  [[maybe_unused]] const void* userParam) {
  // ignore insignificant error/warning codes
  if (id == 131169 || id == 131185 || id == 131218 || id == 131204 ||
      id == 0)  //|| id == 131188 || id == 131186)
    return;

  std::stringstream err_stream;
  err_stream << "OpenGL Debug message (" << id << "): " << message << '\n';

  switch (source) {
    case GL_DEBUG_SOURCE_API:
      err_stream << "Source: API";
      break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
      err_stream << "Source: Window Manager";
      break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
      err_stream << "Source: Shader Compiler";
      break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
      err_stream << "Source: Third Party";
      break;
    case GL_DEBUG_SOURCE_APPLICATION:
      err_stream << "Source: Application";
      break;
    case GL_DEBUG_SOURCE_OTHER:
      err_stream << "Source: Other";
      break;
  }

  err_stream << '\n';

  switch (type) {
    case GL_DEBUG_TYPE_ERROR:
      err_stream << "Type: Error";
      break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
      err_stream << "Type: Deprecated Behaviour";
      break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
      err_stream << "Type: Undefined Behaviour";
      break;
    case GL_DEBUG_TYPE_PORTABILITY:
      err_stream << "Type: Portability";
      break;
    case GL_DEBUG_TYPE_PERFORMANCE:
      err_stream << "Type: Performance";
      break;
    case GL_DEBUG_TYPE_MARKER:
      err_stream << "Type: Marker";
      break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
      err_stream << "Type: Push Group";
      break;
    case GL_DEBUG_TYPE_POP_GROUP:
      err_stream << "Type: Pop Group";
      break;
    case GL_DEBUG_TYPE_OTHER:
      err_stream << "Type: Other";
      break;
  }

  err_stream << '\n';

  switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
      err_stream << "Severity: high";
      spdlog::critical("{}", err_stream.str());
      break;
    case GL_DEBUG_SEVERITY_MEDIUM:
      err_stream << "Severity: medium";
      spdlog::error("{}", err_stream.str());
      break;
    case GL_DEBUG_SEVERITY_LOW:
      err_stream << "Severity: low";
      spdlog::warn("{}", err_stream.str());
      break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
      err_stream << "Severity: notification";
      spdlog::debug("{}", err_stream.str());
      break;
  }
}
}  // namespace gfx
