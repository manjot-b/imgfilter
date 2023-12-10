target_sources(${CMAKE_PROJECT_NAME} PRIVATE
	App.cpp
	App.hpp
	CanvasWindow.cpp
	CanvasWindow.hpp
	FilterParams.hpp
	FilterParamsWindow.cpp
	FilterParamsWindow.hpp
	Image.cpp
	Image.hpp
	IWindow.hpp
	MainWindow.cpp
	MainWindow.hpp
	PreviewWindow.cpp
	PreviewWindow.hpp
	ProfilerInfo.cpp
	ProfilerInfo.hpp
	ProfilerWindow.cpp
	ProfilerWindow.hpp
)

add_subdirectory(imagefilter)
