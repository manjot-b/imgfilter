cmake_minimum_required(VERSION 3.21)

# Compute backend settings
if(NOT DEFINED COMPUTE_BACKEND)
	set(COMPUTE_BACKEND "ROCM")
endif()

string(TOUPPER ${COMPUTE_BACKEND} COMPUTE_BACKEND)
if(COMPUTE_BACKEND STREQUAL "ROCM")
	message("ROCM backend. Enable HIP.")
	enable_language(HIP) # must be enabled before project() command
endif()

project(imgfilter)

set(CMAKE_CXX_STANDARD 17)
add_executable(${CMAKE_PROJECT_NAME} "")

target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -Wall -Werror)
if(COMPUTE_BACKEND STREQUAL "ROCM")
	target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -Wno-pragma-once-outside-header)
endif()

set(SOURCE_DIR "src")
add_subdirectory(${SOURCE_DIR})
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${SOURCE_DIR})

set(DEPENDENCIES_DIR "deps")
add_subdirectory(${DEPENDENCIES_DIR})
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${DEPENDENCIES_DIR})

get_target_property(SOURCE_FILES ${CMAKE_PROJECT_NAME} SOURCES)
set_source_files_properties(${SOURCE_FILES} PROPERTIES LANGUAGE HIP)

set(IMGS_DIR "imgs")
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/${IMGS_DIR} ${CMAKE_BINARY_DIR}/${IMGS_DIR})
