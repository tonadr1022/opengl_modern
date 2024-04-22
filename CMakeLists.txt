cmake_minimum_required(VERSION 3.21)

include("${CMAKE_CURRENT_LIST_DIR}/cmake/vcpkg.cmake")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(opengl_modern VERSION 1.0)

add_executable(${PROJECT_NAME} src/main.cpp src/Bar.cpp)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(spdlog CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE
    spdlog::spdlog
    glm::glm
    glfw
    EnTT::EnTT
    )

target_include_directories(${PROJECT_NAME} PRIVATE "src")
