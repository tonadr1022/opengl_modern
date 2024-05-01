project(opengl_renderer)

set(HEADERS)

set(SOURCES src/systems.cpp src/main.cpp src/scene_main.cpp)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC opengl_modern)

target_link_libraries(
  ${PROJECT_NAME}
  opengl_modern
  spdlog::spdlog
  )
