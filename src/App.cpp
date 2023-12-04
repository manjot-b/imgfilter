#include "App.hpp"

#include <filesystem>
#include <iostream>

#include "ImageFilter.hpp"
#include "PreviewWindow.hpp"

App::App() : m_mainWindow(MainWindow::Get()), m_lastSelectedThumbnailFilter(ImageFilter::Filter::None)
{
	// Create the main window so the the GLFW and OpenGL contexts
	// get created before using them.

	using namespace std::placeholders;

	m_filterParams.m_sepia.k = 1.f;
	m_filterParams.m_boxBlur.filterDim = 3;
	m_filterParams.m_gausBlur.filterDim = 3;
	m_filterParams.m_gausBlur.sigma = 1.f;

	PreviewWindow::ThumbnailSelectFunction thumbnailSelectCallback = std::bind(&App::OnThumbnailSelect, this, _1, _2);
	m_mainWindow.GetPreviewWindow()->AddThumbnailSelectCallback(thumbnailSelectCallback);
}

App& App::Get()
{
	static App app;
	return app;
}

void App::Run()
{
	while(m_mainWindow.ReadyToClose() == false)
	{
		m_mainWindow.Render(m_filterParams);

		if (m_filterParams != m_prevfilterParams)
		{
			// TODO: Run this function asynchronously. Push the filter params onto a queue
			// that is processed in another thread.
			ComputeAndDisplayFilteredImages();
			m_prevfilterParams = m_filterParams;
		}
	}

	m_mainWindow.Close();
}

bool App::LoadImage(const std::string& filepath)
{
	m_originalImage = std::make_shared<Image>(filepath);

	if (m_originalImage->GetData() == nullptr)
	{
		return false;
	}

	m_originalImage->PrintInfo();

	ComputeAndDisplayFilteredImages();

	return true;
}

void App::ComputeAndDisplayFilteredImages()
{
	std::filesystem::path originalPath = std::filesystem::path(m_originalImage->GetFilename());

	std::shared_ptr<PreviewWindow> previewWindow = m_mainWindow.GetPreviewWindow();
	previewWindow->ReplaceThumbnail(ImageFilter::Filter::None, originalPath.stem(), m_originalImage);

	// Extract the stem of the filename from the image. Then, use it to create a thumbnail.
	// The stem is expected to be unique.
	auto filteredImageResults = ImageFilter::GetFilteredImages(m_originalImage, m_filterParams);

	m_mainWindow.GetProfilerWindow()->ClearImageProfilers();

	for (const auto& filteredResult : filteredImageResults)
	{
		ImageFilter::Filter filter = filteredResult.m_filter;
		std::shared_ptr<Image> image = filteredResult.m_image;	
		std::filesystem::path path(image->GetFilename());
		previewWindow->ReplaceThumbnail(filter, path.stem(), image);

		m_mainWindow.GetProfilerWindow()->AddImageProfiler(path.stem(), filteredResult.m_profilerInfo);
	}

	const PreviewWindow::Thumbnail& thumbnail = previewWindow->ThumbnailAt(m_lastSelectedThumbnailFilter);
	m_mainWindow.GetCanvasWindow()->SetImage(thumbnail.m_name, thumbnail.m_image);
}

void App::OnThumbnailSelect(const PreviewWindow::Thumbnail& thumbnail, ImageFilter::Filter filter)
{
	m_mainWindow.GetCanvasWindow()->SetImage(thumbnail.m_name, thumbnail.m_image);
	m_lastSelectedThumbnailFilter = filter;
}

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " <image>\n";
		return 1;
	}

	// Create the app so the the GLFW and OpenGL contexts
	// get created before using them.
	App& app = App::Get();

	std::string imageFilename(argv[1]);
	std::cout << "Reading in " << imageFilename << '\n';
	if (app.LoadImage(imageFilename) == false)
	{
		std::cerr << "Error: Could not read image " << imageFilename << '\n';	
	}

	app.Run();
}
