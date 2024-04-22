project(opengl_modern)

set(HEADERS Bar.h)

set(SOURCES Bar.cpp)

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})

target_link_libraries(opengl_modern PUBLIC spdlog::spdlog glm::glm glfw
                                           EnTT::EnTT)
