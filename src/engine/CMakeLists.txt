project(opengl_modern)

set(SOURCES
    scene.cpp
    application/engine.cpp
    renderer/renderer.cpp
    resource/shader_manager.cpp
    resource/model_loader.cpp
    resource/material_manager.cpp
    resource/resource_manager.cpp
    resource/texture_manager.cpp
    renderer/gl/shader.cpp
    application/input.cpp
    graphics_manager.cpp
    window_manager.cpp
    ecs/system/camera_system.cpp
    ecs/entity/entity.cpp
    renderer/gl/debug.cpp
    renderer/gl/buffer.cpp
    ecs/component/transform.cpp
    ecs/system/imgui_manager.cpp
    util/load_file.cpp
    util/imgui_extensions.cpp
    core/e_assert.cpp
    ecs/component/camera.cpp
    pch.cpp
    util/timer.cpp
    renderer/gl/vertex_array.cpp
    renderer/gl/texture_2d.cpp
    ${CMAKE_SOURCE_DIR}/dep/imgui_file/ImGuiFileDialog.cpp

    # FOR NOW, I'm not separating into a separate library because
    # compilation and linking takes longer
    # when it is.
    test/systems.cpp
    test/main.cpp
    test/scene_main.cpp
    test/scene_2.cpp

    )


# add_library(${PROJECT_NAME} STATIC ${SOURCES})
add_executable(${PROJECT_NAME} ${SOURCES})

target_precompile_headers(${PROJECT_NAME} PRIVATE
                          ${CMAKE_CURRENT_SOURCE_DIR}/pch.h)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE assimp::assimp
          Tracy::TracyClient
          imgui::imgui
          GLEW::GLEW
          spdlog::spdlog
          glm::glm
          glfw
          EnTT::EnTT
          fmt::fmt)
