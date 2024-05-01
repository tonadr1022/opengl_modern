#include "camera.h"

#include <imgui.h>
// void ProjectionMatrixSettings::OnImGuiRender() {
//   float fov_rad = glm::radians(fov);
//   if (ImGui::SliderAngle("FOV", &fov_rad, MinFov, MaxFov)) {
//     fov = glm::degrees(fov_rad);
//   }
//   ImGui::Text("Aspect Ratio: %.3f", aspect_ratio);
// }
//
// void CameraSettings::OnImGuiRender() {
//   ImGui::SliderFloat("Mouse Sensitivity", &mouse_sensitivity, MinMouseSensitivity,
//                      MaxMouseSensitivity);
// }
//
// void ProjectionMatrixSettings::ClampFOV() { fov = glm::clamp(fov, MinFov, MaxFov); }
