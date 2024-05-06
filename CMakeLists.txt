cmake_minimum_required(VERSION 3.21)

include("${CMAKE_CURRENT_LIST_DIR}/cmake/vcpkg.cmake")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(opengl_modern VERSION 1.0)

find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(Tracy CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(TRACY_ENABLE)
add_compile_definitions(SHADER_PATH="${CMAKE_SOURCE_DIR}/resources/shaders/")
add_compile_definitions(TEXTURE_PATH="${CMAKE_SOURCE_DIR}/resources/textures/")
add_compile_definitions(MODEL_PATH="${CMAKE_SOURCE_DIR}/resources/models/")
add_compile_definitions(SRC_PATH="${CMAKE_SOURCE_DIR}/")

include_directories(dep)
include_directories(src)
add_subdirectory(src)

add_subdirectory(test)
