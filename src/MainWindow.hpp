#pragma once

class GLFWwindow;

class MainWindow
{
	private:
		MainWindow();
		static void ErrorCallback(int error, const char* description);
		GLFWwindow* m_window;

		void displayDockingSpace();

	public:
		MainWindow(const MainWindow&) = delete;
		void operator=(const MainWindow&) = delete;

		static MainWindow& Get();
		void Render();
		bool ReadyToClose();
		void Close();
};
