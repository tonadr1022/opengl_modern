project(opengl_modern)

set(SOURCES
    scene.cpp
    application/engine.cpp
    renderer/renderer.cpp
    resource/shader_manager.cpp
    resource/mesh_manager.cpp
    resource/material_manager.cpp
    resource/texture_manager.cpp
    renderer/gl/shader.cpp
    application/input.cpp
    ecs/system/graphics_system.cpp
    ecs/system/window_system.cpp
    ecs/entity/entity.cpp
    renderer/gl/debug.cpp
    renderer/gl/buffer.cpp
    ecs/component/transform.cpp
    ecs/system/imgui_system.cpp
    util/load_file.cpp
    util/imgui_extensions.cpp
    core/e_assert.cpp
    pch.cpp
    util/timer.cpp
    )


add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_precompile_headers(${PROJECT_NAME} PRIVATE
                          ${CMAKE_CURRENT_SOURCE_DIR}/pch.h)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE assimp::assimp
          imgui::imgui
          GLEW::GLEW
          spdlog::spdlog
          glm::glm
          glfw
          EnTT::EnTT
          fmt::fmt)
