target_sources(${CMAKE_PROJECT_NAME} PRIVATE
	glad/glad.c
	glad/glad.h
	KHR/khrplatform.h
	stb/stb_image.h
	stb/stb_image_write.h
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)
target_link_libraries(${CMAKE_PROJECT_NAME} glfw)

add_library(imgui 
	imgui/imgui.cpp
	imgui/imgui.h
	imgui/imgui_demo.cpp
	imgui/imgui_draw.cpp
	imgui/imgui_internal.h
	imgui/imgui_tables.cpp
	imgui/imgui_widgets.cpp
	imgui/backends/imgui_impl_glfw.cpp
	imgui/backends/imgui_impl_glfw.h
	imgui/backends/imgui_impl_opengl3.cpp
	imgui/backends/imgui_impl_opengl3.h
)
set_target_properties(imgui PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(imgui PRIVATE "imgui" "imgui/backends" "glfw/include")
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE "imgui" "imgui/backends")
target_link_libraries(${CMAKE_PROJECT_NAME} imgui)

add_custom_command(TARGET imgui POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/imgui/misc/fonts" "${CMAKE_BINARY_DIR}/fonts")
