cmake_minimum_required(VERSION 3.21)
project(imgmanip LANGUAGES HIP)

set(SOURCE_DIR "src")
add_executable(${CMAKE_PROJECT_NAME} ${SOURCE_DIR}/main.hip)

set (IMGS_DIR "imgs")
add_custom_target(link_images ALL
	COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/${IMGS_DIR} ${CMAKE_BINARY_DIR}/${IMGS_DIR})
