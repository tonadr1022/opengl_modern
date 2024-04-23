project(opengl_modern)

set(SOURCES
    renderer/renderer.cpp
    window.cpp
    ecs/entity/entity.cpp
    ecs/component/transform.cpp
    ecs/system/graphics_system.cpp
    e_assert.cpp
    engine.cpp
    application.cpp)
add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_link_libraries(opengl_modern PRIVATE GLEW::GLEW spdlog::spdlog glm::glm
                                            glfw EnTT::EnTT)
