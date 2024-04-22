project(opengl_modern)

set(HEADERS Bar.h)

set(SOURCES Bar.cpp)

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(spdlog CONFIG REQUIRED)

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})

target_link_libraries(opengl_modern PRIVATE spdlog::spdlog glm::glm glfw
                                            EnTT::EnTT)
