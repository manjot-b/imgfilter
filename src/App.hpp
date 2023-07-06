#pragma once

#include <memory>
#include <string>

#include "Image.hpp"
#include "MainWindow.hpp"

class App
{
	private:
		App();
		MainWindow& m_mainWindow;
		std::shared_ptr<Image> m_originalImage;

	public:
		App(const App&) = delete;
		void operator=(const App&) = delete;

		static App& Get();
		void Run();
		bool LoadImage(const std::string& filepath);
};
