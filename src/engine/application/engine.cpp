#include "engine.h"

#include <imgui.h>

#include "../scene.h"
#include "../timestep.h"
#include "../util/timer.h"
#include "engine/application/event.h"
#include "engine/application/key_codes.h"
#include "engine/ecs/system/graphics_system.h"
#include "engine/ecs/system/imgui_system.h"
#include "engine/ecs/system/window_system.h"
#include "engine/pch.h"
#include "engine/renderer/renderer.h"
#include "engine/resource/shader_manager.h"
#include "engine/util/profiler.h"
#include "input.h"

// Engine* Engine::instance_ = nullptr;

namespace engine {

Engine::Engine() {
  PROFILE_FUNCTION();
  window_system_ = new WindowSystem;
  imgui_system_ = new ImGuiSystem;
  graphics_system_ = new GraphicsSystem;

  window_system_->Init();
  window_system_->SetUserPointer(this);

  // TODO(tony): global variable system
  window_system_->SetVsync(true);

  graphics_system_->Init();

  imgui_system_->Init(window_system_->GetContext());

  Input::init_glfw_input_callbacks(window_system_->GetContext());
}

void Engine::OnEvent(const Event& e) {
  switch (e.type) {
    case EventType::KeyPressed:
      if (e.key.code == KeyCode::Q && e.key.system) {
        Stop();
        return;
      } else if (e.key.code == KeyCode::G && e.key.control) {
        draw_imgui_ = !draw_imgui_;
        return;
      }
    default:
      break;
  }
  active_scene_->OnEvent(e);
}

// Engine& Engine::Get() {
//   if (instance_ == nullptr) {
//     instance_ = new Engine;
//   }
//   EASSERT(instance_);
//   return *instance_;
// }

void Engine::Run() {
  PROFILE_FUNCTION();
  EASSERT(active_scene_ != nullptr);
  running_ = true;

  Timestep timestep;
  Timer timer;
  double last_time = timer.ElapsedSeconds();
  // const double sim_time = 1.0 / 60.0;
  while (running_ && !window_system_->ShouldClose()) {
    PROFILE_SCOPE("Main Loop");
    Input::Update();

    if (draw_imgui_) imgui_system_->StartFrame();

    auto current_time = timer.ElapsedSeconds();
    double delta_time = current_time - last_time;
    last_time = current_time;
    // for now, give scene simulated time. eventually, have two functions, update and fixed update,
    // or do all fixed update in physics system that doesn't exist.
    double frame_time = delta_time;
    // while (frame_time >= 0) {
    //   double dt = std::min(frame_time, sim_time);
    //   timestep.dt_actual = dt;
    //   frame_time -= dt;
    //   active_scene_->OnUpdate(timestep);
    // }
    {
      PROFILE_SCOPE("Scene Update");
      active_scene_->OnUpdate(timestep);
    }
    timestep.dt_actual = delta_time;
    graphics_system_->StartFrame(*active_scene_);
    graphics_system_->DrawOpaque(active_scene_->registry);
    graphics_system_->EndFrame();

    if (draw_imgui_) {
      ImGuiSystemPerFrame(timestep);
      imgui_system_->EndFrame();
    }

    window_system_->SwapBuffers();
    FrameMark;
  }

  Shutdown();
}
Engine* Engine::instance_ = nullptr;

Engine& Engine::Get() {
  if (!instance_) {
    instance_ = new Engine;
  }
  return *instance_;
}

void Engine::ImGuiSystemPerFrame(Timestep timestep) {
  imgui_system_->RenderRendererStats(gfx::Renderer::GetStats());
  active_scene_->OnImGuiRender();

  ImGui::Begin("Settings");
  bool vsync = window_system_->GetVsync();
  if (ImGui::Checkbox("Vsync", &vsync)) {
    window_system_->SetVsync(vsync);
  }

  if (ImGui::Button("Recompile Shaders")) {
    ShaderManager::RecompileShaders();
  }

  imgui_system_->FramerateSubMenu(timestep);

  ImGui::End();
}

void Engine::Shutdown() {
  imgui_system_->Shutdown();
  graphics_system_->Shutdown();
  window_system_->Shutdown();

  delete imgui_system_;
  delete graphics_system_;
  delete window_system_;
}

Engine::~Engine() = default;

void Engine::Stop() { running_ = false; }

void Engine::LoadScene(std::unique_ptr<Scene> scene) { active_scene_ = std::move(scene); }

}  // namespace engine
