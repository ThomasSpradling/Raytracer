file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS
    src/*.cpp src/*.h)

add_library(raytracer STATIC ${CORE_SOURCES})

target_include_directories(raytracer
  PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(raytracer
        glm::glm
        glfw
        assimp::assimp
        volk::volk
        VulkanMemoryAllocator
)
