project(opengl_modern)

set(SOURCES
window.cpp
scene.cpp
engine.cpp
renderer/renderer.cpp
renderer/resource/shader_manager.cpp
renderer/resource/mesh_manager.cpp
renderer/resource/texture_manager.cpp
renderer/gl/shader.cpp
input.cpp
application.cpp
ecs/system/graphics_system.cpp
ecs/entity/entity.cpp
ecs/component/transform.cpp
e_assert.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_link_libraries(opengl_modern PRIVATE GLEW::GLEW spdlog::spdlog glm::glm
                                            glfw EnTT::EnTT)
