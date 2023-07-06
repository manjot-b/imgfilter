#pragma once

#include <imgui.h>
#include <memory>
#include <vector>

#include "IWindow.hpp"
#include "Image.hpp"
#include "CanvasWindow.hpp"
#include "PreviewWindow.hpp"
#include "ProfilerWindow.hpp"

class GLFWwindow;

class MainWindow : public IWindow
{
	private:
		MainWindow();
		static void ErrorCallback(int error, const char* description);
		GLFWwindow* m_window;

		std::shared_ptr<CanvasWindow> m_canvasWindow;
		std::shared_ptr<PreviewWindow> m_previewWindow;
		std::shared_ptr<ProfilerWindow> m_profilerWindow;
		std::vector<std::shared_ptr<IWindow>> m_windows;

		std::shared_ptr<Image> m_originalImage;

		void displayDockingSpace();

	public:
		MainWindow(const MainWindow&) = delete;
		void operator=(const MainWindow&) = delete;

		static MainWindow& Get();
		virtual void Render() override;
		bool ReadyToClose();
		void Close();
		std::shared_ptr<CanvasWindow> GetCanvasWindow();
		std::shared_ptr<PreviewWindow> GetPreviewWindow();
		std::shared_ptr<ProfilerWindow> GetProfilerWindow();
};
