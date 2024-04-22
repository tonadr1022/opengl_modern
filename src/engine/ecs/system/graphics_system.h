#include <GLFW/glfw3.h>
class GraphicsSystem {
 public:
  void Init();
  void Shutdown();

 private:
  GLFWwindow* window_;
};
