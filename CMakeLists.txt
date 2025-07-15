cmake_minimum_required(VERSION 3.21)
project(leper)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(SRC_DIR "${ROOT_DIR}/src")
set(THIRD_PARTY_DIR "${ROOT_DIR}/third_party")
add_compile_definitions(SHADER_DIR="${CMAKE_SOURCE_DIR}/resources/shaders")

# -- Leper --
file(GLOB_RECURSE SRC_FILES "${SRC_DIR}/*.c" "${SRC_DIR}/*.cpp")
add_executable(leper ${SRC_FILES})

# -- Dependencies --
find_package(spdlog REQUIRED)

# GLFW
set(GLFW_BUILD_DOCS
    OFF
    CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS
    OFF
    CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES
    OFF
    CACHE BOOL "" FORCE)
add_subdirectory("${THIRD_PARTY_DIR}/glfw")
add_subdirectory("${THIRD_PARTY_DIR}/glm")

# -- --
target_include_directories(
  leper
  PUBLIC "${ROOT_DIR}/include"
)
target_link_libraries(
  leper
  PUBLIC glfw
         spdlog::spdlog
         glm::glm
)
