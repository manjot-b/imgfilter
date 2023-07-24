#pragma once

#include <memory>
#include <string>

#include "Image.hpp"
#include "FilterParams.hpp"
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
		void OnThumbnailSelect(const Thumbnail& thumbnail, uint index);

	private:
		App();
		MainWindow& m_mainWindow;
		std::shared_ptr<Image> m_originalImage;
		uint m_lastSelectedThumbnailIdx;
		FilterParams m_filterParams;
		FilterParams m_prevfilterParams;
};
