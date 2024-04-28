
namespace gfx {

// adapted from https://learnopengl.com/In-Practice/Debugging
extern void MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                            GLsizei /*length*/, const GLchar* message, const void* /*userParam*/);

extern void GLAPIENTRY GLMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                                         [[maybe_unused]] GLsizei length, const GLchar* message,
                                         [[maybe_unused]] const void* userParam);
}  // namespace gfx
