cmake_minimum_required(VERSION 3.21)

include("${CMAKE_CURRENT_LIST_DIR}/cmake/vcpkg.cmake")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(opengl_modern VERSION 1.0)

add_executable(${PROJECT_NAME} src/main.cpp src/test/bar.cpp src/test.cpp)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm glfw)

target_include_directories(${PROJECT_NAME} PRIVATE "src")
