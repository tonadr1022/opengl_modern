#include <engine/application/engine.h>

class Demo {
 public:
  explicit Demo(Engine* engine);

 private:
  void Init();
  Engine* engine_{nullptr};
};
