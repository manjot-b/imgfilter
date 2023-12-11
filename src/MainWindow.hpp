#pragma once

#include "IWindow.hpp"

#include <imgui.h>
#include <memory>
#include <vector>

#include "CanvasWindow.hpp"
#include "FilterParams.hpp"
#include "FilterParamsWindow.hpp"
#include "Image.hpp"
#include "PreviewWindow.hpp"
#include "ProfilerWindow.hpp"

struct GLFWwindow;

class MainWindow : public IWindow
{
	public:
		MainWindow(const MainWindow&) = delete;
		void operator=(const MainWindow&) = delete;

		static MainWindow& Get();
		virtual void Render(FilterParams& filterParams) override;
		bool ReadyToClose();
		void Close();
		std::shared_ptr<CanvasWindow> GetCanvasWindow();
		std::shared_ptr<PreviewWindow> GetPreviewWindow();
		std::shared_ptr<ProfilerWindow> GetProfilerWindow();

	private:
		MainWindow();
		static void ErrorCallback(int error, const char* description);
		GLFWwindow* m_window;

		std::shared_ptr<CanvasWindow> m_canvasWindow;
		std::shared_ptr<PreviewWindow> m_previewWindow;
		std::shared_ptr<ProfilerWindow> m_profilerWindow;
		std::shared_ptr<FilterParamsWindow> m_filterParamsWindow;
		std::vector<std::shared_ptr<IWindow>> m_windows;

		std::shared_ptr<Image> m_originalImage;

		void displayDockingSpace();
		static void onWindowContentScale(GLFWwindow* window, float xScale, float yScale);
};
