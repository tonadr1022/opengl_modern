project(opengl_renderer)

set(HEADERS)

set(SOURCES src/main.cpp src/scene_main.cpp)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC opengl_modern)

target_link_libraries(
  ${PROJECT_NAME}
  opengl_modern
  imgui::imgui
  OpenGL::GL
  GLEW::GLEW
  glfw
  fmt::fmt
  glm::glm)
