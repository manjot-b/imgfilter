cmake_minimum_required(VERSION 3.21)
project(imgfilter LANGUAGES HIP)

set(CMAKE_CXX_STANDARD 17)
add_executable(${CMAKE_PROJECT_NAME} "")
enable_language(CXX)

target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -Wall -Werror -Wno-pragma-once-outside-header)

set(SOURCE_DIR "src")
add_subdirectory(${SOURCE_DIR})

set(DEPENDENCIES_DIR "deps")
add_subdirectory(${DEPENDENCIES_DIR})
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${DEPENDENCIES_DIR})

get_target_property(SOURCE_FILES ${CMAKE_PROJECT_NAME} SOURCES)
set_source_files_properties(${SOURCE_FILES} PROPERTIES LANGUAGE HIP)

set(IMGS_DIR "imgs")
add_custom_target(link_images ALL
	COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/${IMGS_DIR} ${CMAKE_BINARY_DIR}/${IMGS_DIR})
