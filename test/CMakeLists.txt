project(testMain)

set(HEADERS)

set(SOURCES src/main.cpp)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})

target_link_libraries(${PROJECT_NAME} opengl_modern)
