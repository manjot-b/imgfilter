target_sources(${CMAKE_PROJECT_NAME} PRIVATE
	ImageFilter.hpp
	ImageFilter_detail.hpp
)

# TODO: Add the appropriate backend compute based of off value e.g. COMPUTE_BACKEND
add_subdirectory(rocm)
