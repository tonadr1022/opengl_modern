#pragma once

struct ProjectionMatrixSettings {
  float fov{90.f};
  float aspect_ratio{1.f};
  float near_plane{0.1f};
  float far_plane{1000.f};

  void ClampFOV();
  void OnImGuiRender();
  static constexpr const float DefaultFov = 45.0f;
  static constexpr const float MaxFov = 120.0f;
  static constexpr const float MinFov = 5.0f;
};

struct CameraSettings {
  float mouse_sensitivity;
  static constexpr const float DefaultMouseSensitivity = 0.1f;
  static constexpr const float MinMouseSensitivity = 0.01f;
  static constexpr const float MaxMouseSensitivity = 1.0f;
  void OnImGuiRender();
};
