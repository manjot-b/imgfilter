cmake_minimum_required(VERSION 3.21)
project(imgmanip LANGUAGES HIP)

add_executable(${CMAKE_PROJECT_NAME} "")

set(SOURCE_DIR "src")
add_subdirectory(${SOURCE_DIR})

get_target_property(SOURCE_FILES ${CMAKE_PROJECT_NAME} SOURCES)
set_source_files_properties(${SOURCE_FILES} PROPERTIES LANGUAGE HIP)

set (IMGS_DIR "imgs")
add_custom_target(link_images ALL
	COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/${IMGS_DIR} ${CMAKE_BINARY_DIR}/${IMGS_DIR})
