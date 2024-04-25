project(opengl_modern)

# set(HEADERS application/window.h scene.h application/engine.h
# renderer/renderer.h renderer/resource/shader_manager.h
# renderer/resource/mesh_manager.h renderer/resource/texture_manager.h
# renderer/gl/shader.h application/input.h ecs/system/graphics_system.h
# ecs/entity/entity.h ecs/component/transform.h core/e_assert.h pch.h
# util/timer.h)

set(SOURCES
    application/window.cpp
    scene.cpp
    application/engine.cpp
    renderer/renderer.cpp
    renderer/resource/shader_manager.cpp
    renderer/resource/mesh_manager.cpp
    renderer/resource/texture_manager.cpp
    renderer/gl/shader.cpp
    application/input.cpp
    ecs/system/graphics_system.cpp
    ecs/entity/entity.cpp
    ecs/component/transform.cpp
    core/e_assert.cpp
    pch.cpp
    util/timer.cpp)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_precompile_headers(${PROJECT_NAME} PRIVATE
                          ${CMAKE_CURRENT_SOURCE_DIR}/pch.h)

target_link_libraries(${PROJECT_NAME} PRIVATE GLEW::GLEW spdlog::spdlog
                                              glm::glm glfw EnTT::EnTT)
