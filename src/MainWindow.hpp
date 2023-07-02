#pragma once

#include <imgui.h>

class GLFWwindow;

class MainWindow
{
	private:
		MainWindow();
		static void ErrorCallback(int error, const char* description);
		GLFWwindow* m_window;

		const char* m_canvasWindowName;
		const char* m_previewWindowName;
		const char* m_profilerWindowName;

		void displayDockingSpace();
		void displayCanvas();
		void displayImagePreview();
		void displayProfiler();

	public:
		MainWindow(const MainWindow&) = delete;
		void operator=(const MainWindow&) = delete;

		static MainWindow& Get();
		void Render();
		bool ReadyToClose();
		void Close();
};
