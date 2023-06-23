target_sources(${CMAKE_PROJECT_NAME} PRIVATE
	stb/stb_image.h
	stb/stb_image_write.h
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)
target_link_libraries(${CMAKE_PROJECT_NAME} glfw)
