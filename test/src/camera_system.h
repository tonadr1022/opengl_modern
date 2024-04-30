
struct Event;
struct Timestep;

#include <engine/pch.h>

struct CameraSystem {
  void Update(Timestep timestep);
  void OnEvent(const Event& e);
  void OnImGuiRender();

 private:
  void OnMouseMoved(float x_offset, float y_offset);
};
