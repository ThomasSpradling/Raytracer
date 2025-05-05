cmake_minimum_required(VERSION 3.14)
project(RayTracer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Dependencies

add_compile_definitions(STRONK_RESOURCE_PATH="${CMAKE_SOURCE_DIR}/resources")

find_package(Vulkan REQUIRED)

## GLFW

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
add_subdirectory(third_party/glfw)

## Assimp

set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
add_subdirectory(third_party/assimp)

## GLM

set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)
add_subdirectory(third_party/glm)

## Volk

add_subdirectory(third_party/volk)

## ImGui

include_directories(third_party/imgui)

## STB

include_directories(third_party/stb)

## Vulkan Memory Allocator

add_subdirectory(third_party/vma)

# Subdirectories

add_subdirectory(App)
add_subdirectory(RayTracer)
