#include "engine.h"

#include <imgui.h>

#include "../scene.h"
#include "../util/timer.h"
#include "engine/application/event.h"
#include "engine/application/key_codes.h"
#include "engine/core/base.h"
#include "engine/ecs/system/imgui_manager.h"
#include "engine/graphics_manager.h"
#include "engine/renderer/renderer.h"
#include "engine/resource/material_manager.h"
#include "engine/resource/model_manager.h"
#include "engine/resource/shader_manager.h"
#include "engine/window_manager.h"
#include "input.h"

// Engine* Engine::instance_ = nullptr;

namespace engine {

Engine::Engine() {
  window_manager_ = new WindowManager;
  imgui_system_ = new ImGuiManager;
  shader_manager_ = new ShaderManager;
  renderer_ = new gfx::Renderer;
  material_manager_ = new MaterialManager;
  resource_manager_ = new ModelManager;
  graphics_system_ = new GraphicsManager;

  window_manager_->Init();
  window_manager_->SetUserPointer(this);
  // TODO(tony): global variable system
  window_manager_->SetVsync(true);
  graphics_system_->Init();
  material_manager_->Init();
  imgui_system_->Init(window_manager_->GetContext());

  Input::init_glfw_input_callbacks(window_manager_->GetContext());
}

void Engine::OnEvent(const Event& e) {
  switch (e.type) {
    case EventType::kKeyPressed:
      if (e.key.code == KeyCode::Q && e.key.system) {
        Stop();
        return;
      } else if (e.key.code == KeyCode::G && e.key.control) {
        draw_imgui_ = !draw_imgui_;
        return;
      } else if (e.key.code == KeyCode::P) {
        // window_system_->SetCursorVisible(!window_system_->GetCursorVisible());
      } else if (e.key.code == KeyCode::R && e.key.control) {
        ShaderManager::Get().RecompileShaders();
        return;
      }
    default:
      break;
  }
  active_scene_->OnEvent(e);
}

void Engine::Run() {
  ZoneScoped;
  EASSERT(active_scene_ != nullptr);
  running_ = true;

  Timer timer;
  double last_time = timer.ElapsedSeconds();
  Timestep timestep;
  // const double sim_time = 1.0 / 60.0;
  while (running_ && !window_manager_->ShouldClose()) {
    {
      ZoneScopedN("input update");
      Input::Update();
    }

    {
      ZoneScopedN("ImGui Start frame");
      if (draw_imgui_) imgui_system_->StartFrame();
    }
    auto current_time = timer.ElapsedSeconds();
    double delta_time = current_time - last_time;
    last_time = current_time;
    double frame_time = delta_time;
    {
      ZoneScopedN("scene update");
      active_scene_->OnUpdate(timestep);
    }
    {
      ZoneScopedN("Graphics Update");
      timestep = delta_time;
      graphics_system_->StartFrame(*active_scene_);
      graphics_system_->DrawOpaque(*active_scene_);
      graphics_system_->EndFrame();
    }

    {
      ZoneScopedN("imgui");
      if (draw_imgui_) {
        ImGuiSystemPerFrame(timestep);
        imgui_system_->EndFrame();
      }
    }

    {
      ZoneScopedN("swap buffers");
      TracyGpuZone("swap");

      window_manager_->SwapBuffers();
      TracyGpuCollect;
    }
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

void Engine::OnFrameBufferResize(uint32_t width, uint32_t height) {
  renderer_->OnFrameBufferResize(width, height);
}

void Engine::ImGuiSystemPerFrame(Timestep timestep) {
  graphics_system_->OnImGuiRender();
  active_scene_->OnImGuiRender();

  ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_NoNavFocus);
  bool vsync = window_manager_->GetVsync();
  if (ImGui::Checkbox("Vsync", &vsync)) {
    window_manager_->SetVsync(vsync);
  }

  if (ImGui::Button("Recompile Shaders")) {
    shader_manager_->RecompileShaders();
  }

  imgui_system_->FramerateSubMenu(timestep);

  ImGui::End();
}

void Engine::Shutdown() {
  imgui_system_->Shutdown();
  graphics_system_->Shutdown();
  renderer_->Shutdown();
  window_manager_->Shutdown();

  //  may switch unique pointers, but this works for now, lifetime is well defined
  delete material_manager_;
  delete resource_manager_;
  delete imgui_system_;
  delete graphics_system_;
  delete renderer_;
  delete window_manager_;
}

Engine::~Engine() = default;

void Engine::Stop() { running_ = false; }

void Engine::LoadScene(std::unique_ptr<Scene> scene) {
  ZoneScopedN("load scene");
  // reset resources and renderer state only if scene already exists
  bool scene_exists = active_scene_ != nullptr;
  if (scene_exists) {
    // need to reset graphics system before material manager, otherwise
    // the default material will be added before the reset of the material buffer.
    // this is a potential architectural improvement for changing scenes.
    graphics_system_->ResetOnSceneChange(*scene);
    material_manager_->Reset();
  }

  active_scene_ = std::move(scene);
  active_scene_->Init();
}

}  // namespace engine
