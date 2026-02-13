cmake_minimum_required(VERSION 3.20)
project(WaterSimulation LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# GLFW — downloaded automatically
FetchContent_Declare(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

find_package(OpenGL REQUIRED)

add_executable(WaterSimulation
    src/main.cpp
    src/gl_loader.cpp
    src/simulation.cpp
    src/renderer.cpp
)

target_link_libraries(WaterSimulation PRIVATE glfw OpenGL::GL)
target_include_directories(WaterSimulation PRIVATE src)
