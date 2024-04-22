project(opengl_modern)

# set(HEADERS renderer/renderer.h ecs/entity/entity.h ecs/component/transform.h
# application.h)

set(SOURCES e_assert.cpp renderer/renderer.cpp ecs/entity/entity.cpp
            ecs/component/transform.cpp application.cpp)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_link_libraries(opengl_modern PRIVATE glad::glad spdlog::spdlog glm::glm
                                            glfw EnTT::EnTT)
