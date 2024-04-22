cmake_minimum_required(VERSION 3.21)

include("${CMAKE_CURRENT_LIST_DIR}/cmake/vcpkg.cmake")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(opengl_modern VERSION 1.0)

# target_compile_features(opengl_modern PUBLIC cxx_std_20)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(spdlog CONFIG REQUIRED)

add_subdirectory(src)
target_include_directories(opengl_modern PRIVATE "src")

add_subdirectory(test)
