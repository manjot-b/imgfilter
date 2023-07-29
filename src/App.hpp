#pragma once

#include <memory>
#include <string>

#include "FilterParams.hpp"
#include "Image.hpp"
#include "ImageFilter.hpp"
#include "MainWindow.hpp"

class App
{
	public:
		App(const App&) = delete;
		void operator=(const App&) = delete;

		static App& Get();
		void Run();
		bool LoadImage(const std::string& filepath);
		void ComputeAndDisplayFilteredImages();
		void OnThumbnailSelect(const PreviewWindow::Thumbnail& thumbnail, ImageFilter::Filter filter);

	private:
		App();
		MainWindow& m_mainWindow;
		std::shared_ptr<Image> m_originalImage;
		ImageFilter::Filter m_lastSelectedThumbnailFilter;
		FilterParams m_filterParams;
		FilterParams m_prevfilterParams;
};
